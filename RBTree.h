
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
        std::cout << "Deleting: " << *m_key << std::endl;
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
        std::cout << "killing empty node" << std::endl;
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
                std::cout << "Updating root node - new node: " << *child->m_key << std::endl;
            }
            else {
                std::cout << "Updaing root node to an empty node" << std::endl;
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
            std::cout << "Killing root node. New root node: '" << *successor->m_key << "'" << std::endl;
        }
        else {
            std::cout << "New root node - empty" << std::endl;
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
    if (p->m_l == m_self.lock()) {
        return direction::LEFT;
    }
    return direction::RIGHT;
}

template <class T>
void RBTree<T>::RBNode::redden() {
    std::cout << "332" << std::endl;
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
        std::cout << "Grandpa: " << *grandpa->m_key << std::endl;
        direction parent_dir = p->getDirectionFromParent();
        auto uncle = grandpa->getChild(flip(parent_dir));
        std::cout << "352" << std::endl;
        if (uncle->m_color == color::RED) {
            std::cout << "354" << std::endl;
            // Red father, red uncle
            p->m_color = color::BLACK;
            uncle->m_color = color::BLACK;
            grandpa->redden();
            return;
        }
        else {
            std::cout << "362" << std::endl;
            direction my_dir = getDirectionFromParent();
            if (my_dir == parent_dir) {
                std::cout << "365" << std::endl;
                grandpa->rotate(flip(my_dir));
                // As parent was red, grandpa was black. Now parent is the new grandpa
                // and we need to switch colors between it and the old grandpa.
                p->m_color = color::BLACK;
                grandpa->m_color = color::RED;
                return;
            }
            else {
                std::cout << "374" << std::endl;
                // Parent is left child, I am right child
                // or the symmetrical case
                p->rotate(parent_dir);
                std::cout << "375" << std::endl;
                // I am now parent of parent,
                // set up conditions to a recursive call
                p->m_color = color::BLACK;
                std::cout << "376" << std::endl;
                p->redden();
                std::cout << "377" << std::endl;
                return;
            }
        }
    }
    else {
        std::cout << "387" << std::endl;
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
        std::cout << "RB" << std::endl;
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
            std::cout << "BBBB" << std::endl;
            // Black brother with 2 black kids
            // Remove double black status from current node,
            // and turn brother red. Then blacken parent
            brother->m_color = color::BLACK;
            p->blacken();
            return;
        }
        if (brother->getChild(dir)->m_color == color::RED && brother->getChild(flip(dir))->m_color == color::BLACK) {
            std::cout << "BBRBK" << std::endl;
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
        std::cout << "BBRFC" << std::endl;
        brother->m_color = p->m_color;
        p->m_color = color::BLACK;
        brother->getChild(flip(dir))->m_color = color::BLACK;
        p->rotate(flip(dir));
        return;
    }
}

template <class T>
void RBTree<T>::RBNode::rotate(direction dir) {
    std::cout << "467" << std::endl;
    auto child = getChild(flip(dir));
    if (!child) {
        throw std::runtime_error("Attempting to rotate a NIL into node!");
    }
    auto tmp_p = m_p;
    m_p = child;
    if (dir == direction::LEFT) {
        std::cout << "475" << std::endl;
        m_r = child->m_l;
        child->m_l = m_self.lock();
    }
    else {
        std::cout << "475" << std::endl;
        m_l = child->m_r;
        child->m_r = m_self.lock();
    }
    std::cout << "484" << std::endl;
    child->m_p = tmp_p;
    auto p = tmp_p.lock();
    if(p) {
        std::cout << "488" << std::endl;
        // This node has a parent, update parent's child pointer
        direction my_dir = getDirectionFromParent();
        if (my_dir == direction::LEFT) {
            std::cout << "492" << std::endl;
            p->m_r = child;
        }
        else {
            std::cout << "496" << std::endl;
            p->m_l = child;
        }
    }
    else {
        std::cout << "499" << std::endl;
        // Rotate child into the root node!
        std::cout << "Noted root change" << std::endl;
        if (child->m_key) {
            std::cout << "New root: " << *(child->m_key) << std::endl;
        }
        else {
            std::cout << "Rotated an empty node to root!" << *(child->m_key) << std::endl;
        }
        m_tree.lock()->m_root = child;
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
    std::cout << "Inserting '" << key << "'" << std::endl;
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
