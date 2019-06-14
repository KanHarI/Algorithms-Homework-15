
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

template <class T>
class RBNode final {
public:
    using comp_func_t = std::function<int(const T&,const T&)>;

    // Operations
    void insert(T);
    void remove(T);

    static std::shared_ptr<RBNode<T>> createRBTree(comp_func_t comp_func);
    RBNode(comp_func_t comp_func);
    ~RBNode();

private:
    void redden();

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
    auto tree = std::make_shared<RBNode<T>>(comp_func);
    tree->m_self = tree;
    return tree;
}

// When creating the first node, it is a black NIL on top of the tree
template <class T>
RBNode<T>::RBNode(comp_func_t comp_func) :
    m_comp_func(comp_func),
    m_color(color::BLACK) {}

template <class T>
RBNode<T>::~RBNode() {}

template <class T>
void RBNode<T>::insert(T key) {
    if (!m_key) { // Node was Nil
        m_key = std::make_unique<T>(key);
        m_l = std::make_shared<RBNode<T>>(m_comp_func);
        m_l->m_p = m_self;
        m_l->m_self = m_l;
        m_r = std::make_shared<RBNode<T>>(m_comp_func);
        m_r->m_p = m_self;
        m_r->m_self = m_r;
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
void RBNode<T>::redden() {
    if(auto p = m_p.lock()) {
        if (p->m_color == color::BLACK) {
            m_color = color::RED;
            return;
        }
        throw NotImplemented("Parent is red, oh no!");
    }
    else { // parent is nonexistant -> this node is the top node
        return;
    }
}
