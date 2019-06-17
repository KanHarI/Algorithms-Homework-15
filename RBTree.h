
#ifndef RBTREE_H
#define RBTREE_H

#include <memory>
#include <functional>

#include "Exceptions.h"

enum class color
{
    RED,
    BLACK
};

enum class direction {
    LEFT,
    RIGHT
};

inline direction flip(direction orig) {
    if (orig == direction::LEFT) {
        return direction::RIGHT;
    }
    return direction::LEFT;
}


template <class T>
class RBTree final {
private:
    struct ctor_protector_t{};

public:
    class RBNode;
    using comp_func_t = std::function<int(T, T)>;
    RBTree(comp_func_t, ctor_protector_t);
    ~RBTree();

    static std::shared_ptr<RBTree<T>> createTree(comp_func_t comp_func);

    void insert(T key);
    void remove(T key);
    bool lookup(T key);

private:
    std::shared_ptr<RBNode> m_root;
    comp_func_t m_comp_func;
};

template <class T>
class RBTree<T>::RBNode final {
public:
    // Inserts a key to subtree under curent node
    void insert(T key);
    // Removes current node from tree. WARNING: you must hold a shared_ptr to
    // current node when calling this.
    void kill();

    // find predecessor node
    std::shared_ptr<RBNode> succ();
    std::shared_ptr<RBNode> pred();
    std::shared_ptr<RBNode> scan(direction dir);

    std::shared_ptr<RBNode> find(T key);

    std::shared_ptr<RBNode> getChild(direction dir);
    direction getDirectionFromParent();

    bool isNil();

    RBNode(std::weak_ptr<RBTree<T>> tree);
    ~RBNode();

    static std::shared_ptr<RBNode> createNode(std::weak_ptr<RBTree<T>> tree);
    std::shared_ptr<RBNode> createChild();

private:
    // Private as these operations can unbalance a tree
    void redden();
    void blacken();
    void rotate(direction dir);

    color m_color;
    std::weak_ptr<RBTree<T>> m_tree; // pointer to tree object
    std::weak_ptr<RBNode> m_self; // weak ptr to self. Used to pass to children nodes
    std::weak_ptr<RBNode> m_p; // parent. Weak ptr to prevent memory leak upon deletion
    std::shared_ptr<RBNode> m_l; // left child
    std::shared_ptr<RBNode> m_r; // right child
    std::unique_ptr<T> m_key;

};

template <class T>
std::shared_ptr<RBTree<T>> RBTree<T>::createTree(comp_func_t comp_func) {
    auto tree = std::make_shared<RBTree<T>>(comp_func, ctor_protector_t());
    tree->m_root = RBNode::createNode(tree);
    return tree;
}

template <class T>
RBTree<T>::RBNode::RBNode(std::weak_ptr<RBTree<T>> tree)
    : m_color(color::BLACK)
    , m_tree(tree) {}

template <class T>
RBTree<T>::RBNode::~RBNode() {
    if (m_key) {
    }
}

template <class T>
std::shared_ptr<typename RBTree<T>::RBNode> RBTree<T>::RBNode::createNode(std::weak_ptr<RBTree<T>> tree) {
    auto node = std::make_shared<RBNode>(tree);
    node->m_self = node;
    return node;
}

template <class T>
std::shared_ptr<typename RBTree<T>::RBNode> RBTree<T>::RBNode::createChild() {
    auto node = createNode(m_tree);
    node->m_p = m_self;
    return node;
}

template <class T>
bool RBTree<T>::RBNode::isNil() {
    return !m_key;
}

template <class T>
void RBTree<T>::RBNode::insert(T key) {
    if (!m_key) { // Node was Nil
        std::cout << "Creating: '" << key << "'" << std::endl;
        if (m_p.lock()) {
            std::cout << "Under: '" << *m_p.lock()->m_key << "'" << std::endl;
        }
        else {
            std::cout << "As root!" << std::endl;
        }
        m_key = std::make_unique<T>(key);
        m_l = createChild();
        m_r = createChild();
        redden();
        return;
    }

    int comp_res = m_tree.lock()->m_comp_func(key, *m_key);

    if (comp_res == 0) {
        throw KeyAlreadyExists();
    }

    if (comp_res > 0) {
        m_r->insert(key);
        return;
    }
    m_l->insert(key);
}

template <class T>
void RBTree<T>::RBNode::kill() {
    // We change the parent's pointer to self.
    // The code does not delete the object mid-execution and does not crash
    // thanks to the only API to this function coming from TBTree<T>::remove
    // which holds a shared_ptr to current element.
    if (!m_key){
        // Attempting to kill leaf, do nothing
        return;
    }
    if(!m_l->m_key && !m_r->m_key) {
        // Node's childs are both leafs, can be removed safely
        auto p = m_p.lock();
        if(p) {
            m_l->m_p = p;
            // Node has a parent
            auto dir = getDirectionFromParent();
            if (dir == direction::LEFT) {
                // Reseting pointer will delete node as this is the only
                // reference to these nodes (using shared_ptr)
                p->m_l = m_l;
            }
            else {
                p->m_r = m_l;
            }
            if (m_color == color::BLACK) {
                // Double black the leaf that replaced me
                m_l->blacken();
            }
        }
        else {
            // Attempting to kill root node which has no kids, reset it's state
            m_tree.lock()->m_root = RBNode::createNode(m_tree);
        }
        return;
    }
    if (!m_l->m_key || !m_r->m_key) {
        // Node has only one child
        auto child = m_l->m_key ? m_l : m_r;
        child->m_p = m_p;
        auto p = m_p.lock();
        if (p) {
            // Node has a parent
            auto dir = getDirectionFromParent();
            if (dir == direction::LEFT) {
                p->m_l = child;
            }
            else {
                p->m_r = child;
            }
        }
        else {
            // Deleting root node
            m_tree.lock()->m_root = child;
            if (child->m_key) {
            }
            else {
            }
        }
        if (m_color == color::BLACK) {
            child->blacken();
        }
        return;
    }
    auto successor = succ();
    // Switch location with successor, then kill this node in that new position
    auto tmp_color = successor->m_color;
    successor->m_color = m_color;
    m_color = tmp_color;
    auto tmp_node = successor->m_l;
    successor->m_l = m_l;
    m_l = tmp_node;
    tmp_node = successor->m_r;
    successor->m_r = m_r;
    m_r = tmp_node;
    // Successor cannot be root, as successot has at most one kid
    // and current node has 2 - impossible situation
    tmp_node = successor->m_p.lock();
    auto succ_dir = successor->getDirectionFromParent();
    if (succ_dir == direction::LEFT) {
        tmp_node->m_l = m_self.lock();
    }
    else {
        tmp_node->m_r = m_self.lock();
    }
    successor->m_p = m_p;
    auto p = m_p.lock();
    if (p) {
        auto dir = getDirectionFromParent();
        if (dir == direction::LEFT) {
            p->m_l = successor;
        }
        else {
            p->m_r = successor;
        }
    }
    else {
        // Current node is root!
        if (successor->m_key) {
        }
        else {
        }
        m_tree.lock()->m_root = successor;
    }
    m_p = tmp_node;
    kill();
}

template <class T>
std::shared_ptr<typename RBTree<T>::RBNode> RBTree<T>::RBNode::succ() {
    return scan(direction::RIGHT);
}

template <class T>
std::shared_ptr<typename RBTree<T>::RBNode> RBTree<T>::RBNode::pred() {
    return scan(direction::RIGHT);
}

template <class T>
std::shared_ptr<typename RBTree<T>::RBNode> RBTree<T>::RBNode::scan(direction dir) {
    auto ptr = getChild(dir);
    if(ptr) {
        // There is a child in the scanned direction, therefore successor is in subtree
        while(ptr->m_key) {
            // progress as much as possible in opposite direction in subtree
            ptr = ptr->getChild(flip(dir));
        }
        // Now we reached a leaf without a value, returning direct parent
        return ptr->m_p.lock();
    }
    ptr = m_self.lock();
    while(ptr->m_p.lock() && ptr->getDirectionFromParent() == dir) {
        ptr = m_p.lock();
    }
    // return either the direct parent, or if this is the last node - a nullptr
    return m_p.lock();
}

template <class T>
std::shared_ptr<typename RBTree<T>::RBNode> RBTree<T>::RBNode::find(T key) {
    if (!m_key) {
        // Return an empty node as key was not found
        return m_self.lock();
    }

    int comp_res = m_tree.lock()->m_comp_func(key, *m_key);

    if (comp_res == 0) {
        return m_self.lock();
    }
    if (comp_res > 0) {
        return m_r->find(key);
    }
    return m_l->find(key);
}

template <class T>
std::shared_ptr<typename RBTree<T>::RBNode> RBTree<T>::RBNode::getChild(direction dir) {
    return (dir == direction::LEFT) ? m_l : m_r;
}

template <class T>
direction RBTree<T>::RBNode::getDirectionFromParent() {
    auto p = m_p.lock();
    if (!p) {
        throw std::runtime_error("Root node is looking for parent :(");
    }
    if (p->m_l == m_self.lock()) {
        return direction::LEFT;
    }
    if (p->m_r == m_self.lock()) {
        return direction::RIGHT;
    }
    throw std::runtime_error("Parent does not know this node :(");
}

template <class T>
void RBTree<T>::RBNode::redden() {
    // I found it more intuitive to implement as recursion then as a loop
    if (m_color == color::RED) {
        throw std::runtime_error("Attempting to redden a red node! Error!");
    }
    auto p = m_p.lock();
    if (p) {
        // This node has a parent, therefore this node is not root
        // color this node as red
        m_color = color::RED;
        if (p->m_color == color::BLACK) {
            // parent is black, we can color this node red and end the recursion
            m_color = color::RED;
            return;
        }
        // Parent is red, therefore it cannot be root, and grandpa exists.
        // Also, grandpa is black.
        auto grandpa = p->m_p.lock();
        auto parent_dir = p->getDirectionFromParent();
        auto uncle = grandpa->getChild(flip(parent_dir));
        if (uncle->m_color == color::RED) {
            // Red father, red uncle
            p->m_color = color::BLACK;
            uncle->m_color = color::BLACK;
            grandpa->redden();
            return;
        }
        else {
            // Parent is red, uncle is black
            if (m_key) {
            }
            else {
            }
            auto my_dir = getDirectionFromParent();
            if (my_dir == parent_dir) {
                // Me and parent are chilren of the same size, parent is red,
                // grandpa is black and uncle is black.
                // Switching colors between parent and grandpa and rotating
                // to rebalance tree:
                p->m_color = color::BLACK;
                grandpa->m_color = color::RED;
                grandpa->rotate(flip(my_dir));
                // Now parent is the new grandpa and we need to switch colors
                // between it and the old grandpa.
                return;
            }
            else {
                // Parent is left child, I am right child
                // or the symmetrical case
                p->rotate(parent_dir);
                // I am now parent of parent,
                // set up conditions to a recursive call
                p->m_color = color::BLACK;
                p->redden();
                return;
            }
        }
    }
    else {
        // parent is nonexistant -> this node is the root, black height is increased by 1
        return;
    }
    throw std::runtime_error("Redden case not found! Error!");
}

template <class T>
void RBTree<T>::RBNode::blacken() {
    // I found it more intuitive to implement as recursion then as a loop
    if(m_color == color::RED) {
        // Blackening a red node is easy
        m_color = color::BLACK;
        return;
    }
    auto p = m_p.lock();
    if (!p) {
        // This is root, reduce black height by one...
        m_color = color::BLACK;
        return;
    }
    // Current node is double black
    auto dir = getDirectionFromParent();
    auto brother = p->getChild(flip(dir));
    if (brother->m_color == color::RED) {
        // Brother is red, therefore father is black.
        // Switch colors betweeb brother and father then rotate,
        // this keeps the extra black node but moves us to one of the other
        // cases upon next recursive call.
        brother->m_color = color::BLACK;
        p->m_color = color::RED;
        p->rotate(dir);
        // Call recursively
        blacken();
        return;
    }
    else {
        // Black brother
        if (brother->m_l->m_color == color::BLACK && brother->m_r->m_color == color::BLACK) {
            // Black brother with 2 black kids
            // Remove double black status from current node,
            // and turn brother red. Then blacken parent
            brother->m_color = color::BLACK;
            p->blacken();
            return;
        }
        if (brother->getChild(dir)->m_color == color::RED && brother->getChild(flip(dir))->m_color == color::BLACK) {
            // Red cousin on same side of brother as self to father, 
            // second cousin black.
            // Make red cousin into brother, and switch colors between it and
            // current brother.
            brother->m_color = color::RED;
            brother->getChild(dir)->m_color = color::BLACK;
            brother->rotate(flip(dir));
            // In next recursion iteration we will have a flipped-side red cousing
            blacken();
            return;
        }
        // The remaining case is black brother with red flipped-side cousin.
        // Assign brother the color of father, then color father black and
        // rotate father above me - getting rid of extra black. Color cousin
        // as black to be rid of extra black in flipped side.
        brother->m_color = p->m_color;
        p->m_color = color::BLACK;
        brother->getChild(flip(dir))->m_color = color::BLACK;
        p->rotate(flip(dir));
        return;
    }
}

template <class T>
void RBTree<T>::RBNode::rotate(direction dir) {
    auto new_parent = getChild(flip(dir));
    if (!new_parent || !new_parent->m_key) {
        throw std::runtime_error("Attempting to rotate a NIL into node!");
    }

    std::cout << "Rotating " << ((dir == direction::LEFT) ? "left" : "right") << ": '" << *m_key << "'" << std::endl;
    std::cout << "new_parent: '" << *new_parent->m_key << "'" << std::endl;

    auto old_parent = m_p.lock();
    auto self = m_self.lock();
    direction my_dir = direction::LEFT;
    if (old_parent) {
        std::cout << "old_parent: '" << *old_parent->m_key << "'" << std::endl;
        my_dir = getDirectionFromParent();
        std::cout << __LINE__ << std::endl;
    }
    else {
        std::cout << "Under root!" << std::endl;
    }

    m_p = new_parent;
    new_parent->m_p = old_parent;
    if (dir == direction::LEFT) {
        m_r = new_parent->m_l;
        new_parent->m_l = self;
    }
    else {
        m_l = new_parent->m_r;
        new_parent->m_r = self;
    }
    if (old_parent) {
        if (my_dir == direction::LEFT) {
            old_parent->m_l = new_parent;
        }
        else {
            old_parent->m_r = new_parent;
        }
    }
    else {
        m_tree.lock()->m_root = new_parent;
    }
}


template <class T>
RBTree<T>::RBTree(comp_func_t comp_func, ctor_protector_t ctor_protector)
    : m_comp_func(comp_func) {
    // prevent warnings while not allow user to call Ctor
    ctor_protector = ctor_protector;
}

template <class T>
RBTree<T>::~RBTree() {}

template <class T>
void RBTree<T>::insert(T key) {
    m_root->insert(key);
}

template <class T>
void RBTree<T>::remove(T key) {
    auto node = m_root->find(key);
    if (node->isNil()) {
        throw KeyNotFound();
    }
    node->kill();
}

template <class T>
bool RBTree<T>::lookup(T key) {
    return !m_root->find(key)->isNil();
}

#endif
