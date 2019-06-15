
#include <memory>
#include <exception>
#include <functional>


#define DEF_EXCEPTION(superclass, exception) \
    class exception : public superclass { \
    public: \
        exception(std::string message="") : superclass(message) {} \
    };

DEF_EXCEPTION(std::runtime_error, KeyAlreadyExists);
DEF_EXCEPTION(std::runtime_error, KeyNotFound);
DEF_EXCEPTION(std::runtime_error, NotImplemented);

enum class color
{
    RED,
    BLACK
};

enum class direction {
    LEFT,
    RIGHT
};

direction flip(direction orig) {
    if (orig == direction::LEFT) {
        return direction::RIGHT;
    }
    return direction::LEFT;
}

template <class T>
class RBNode final {
    struct ctor_protector_t{};
public:
    using comp_func_t = std::function<int(const T&,const T&)>;

    // Operations
    void insert(T);
    void remove(T);

    std::shared_ptr<RBNode<T>> getChild(direction dir);
    direction getDirectionFromParent();

    static std::shared_ptr<RBNode<T>> createRBTree(comp_func_t comp_func);
    RBNode(comp_func_t comp_func, ctor_protector_t ctor_protector);
    ~RBNode();

private:
    // Private as these operations can unbalance a tree
    void redden();
    void rotate(direction dir);

    comp_func_t m_comp_func; // function for comparison between 2 keys
    color m_color;
    std::weak_ptr<RBNode<T>> m_self; // weak ptr to self. Used to pass to children nodes
    std::weak_ptr<RBNode<T>> m_p; // parent. Weak ptr to prevent memory leak upon deletion
    std::shared_ptr<RBNode<T>> m_l; // left child
    std::shared_ptr<RBNode<T>> m_r; // right child
    std::unique_ptr<T> m_key;
};

template <class T>
std::shared_ptr<RBNode<T>> RBNode<T>::createRBTree(typename RBNode<T>::comp_func_t comp_func) {
    ctor_protector_t ctor_protector;
    auto tree = std::make_shared<RBNode<T>>(comp_func, ctor_protector);
    tree->m_self = tree;
    return tree;
}

// When creating the first node, it is a black NIL on top of the tree
template <class T>
RBNode<T>::RBNode(comp_func_t comp_func, typename RBNode<T>::ctor_protector_t ctor_protector) :
    m_comp_func(comp_func),
    m_color(color::BLACK) {
        // Unused parameter, statement is to prevent warnings
        ctor_protector = ctor_protector;
    }

template <class T>
RBNode<T>::~RBNode() {}

template <class T>
void RBNode<T>::insert(T key) {
    if (!m_key) { // Node was Nil
        m_key = std::make_unique<T>(key);
        m_l = createRBTree(m_comp_func);

        // Setting the child's parent "informs" them that they are not the root
        m_l->m_p = m_self;
        m_r = createRBTree(m_comp_func);
        m_r->m_p = m_self;
        redden();
        return;
    }
    int comp_res = m_comp_func(key, *m_key);

    if (comp_res > 0) {
        m_l->insert(key);
        return;
    }
    if (comp_res < 0) {
        m_r->insert(key);
        return;
    }
    throw KeyAlreadyExists();
}

template <class T>
std::shared_ptr<RBNode<T>> RBNode<T>::getChild(direction dir) {
    return (dir == direction::LEFT) ? m_l : m_r;
}

template <class T>
direction RBNode<T>::getDirectionFromParent() {
    auto p = m_p.lock();
    if (p->m_l == m_self.lock()) {
        return direction::LEFT;
    }
    return direction::RIGHT;
}

template <class T>
void RBNode<T>::redden() {
    if (m_color == color::RED) {
        return;
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
        // parent is red, therefore it cannot be root, and grandpa exists.
        auto grandpa = p->m_p.lock();
        direction parent_dir = p->getDirectionFromParent();
        auto uncle = grandpa->getChild(flip(parent_dir));
        if (uncle->m_color == color::RED) {
            // Red father, red uncle
            p->m_color = color::BLACK;
            uncle->m_color = color::BLACK;
            grandpa->redden();
            return;
        }
        else {
            direction my_dir = getDirectionFromParent();
            if (my_dir == parent_dir) {
                grandpa->rotate(flip(my_dir));
                // As parent was red, grandpa was black. Now parent is the new grandpa
                // and we need to switch colors between it and the old grandpa.
                p->m_color = color::BLACK;
                grandpa->m_color = color::RED;
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
}

template <class T>
void RBNode<T>::rotate(direction dir) {
    auto child = getChild(dir);
    if (!getChild(flip(dir))) {
        throw std::runtime_error("Attempting to rotate a NIL into node!");
    }
    auto p = m_p.lock();
    m_p = child;
    if (dir == direction::LEFT) {
        m_r = child->m_l;
        child->m_l = m_self.lock();
    }
    else {
        m_l = child->m_r;
        child->m_r = m_self.lock();
    }
    if(p) {
        // This node has a parent, update parent's child pointer
        direction my_dir = getDirectionFromParent();
        if (my_dir == direction::LEFT) {
            p->m_r = child;
        }
        else {
            p->m_l = child;
        }
    }
}
