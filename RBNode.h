
#include <memory>
#include <exception>
#include <functional>


#define DEF_EXCEPTION(superclass, exception) \
    class exception : public superclass { \
    public: \
        exception(std::string message) : superclass(message) {} \
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
    RBNode(comp_func_t comp_func);
    ~RBNode();

    // Operations
    void insert(T);
    void remove(T);

private:
    void recolor(color new_color);

    comp_func_t m_comp_func; // function for comparison between 2 keys
    bool m_is_top;
    color m_color;
    std::weak_ptr<RBNode<T>> m_p; // parent. Weak ptr to prevent memory leak upon deletion
    std::shared_ptr<RBNode<T>> m_l; // left child
    std::shared_ptr<RBNode<T>> m_r; // right child
    std::unique_ptr<T> m_key;
};

// When creating the first node, it is a black NIL on top of the tree
template <class T>
RBNode<T>::RBNode(comp_func_t comp_func) : m_comp_func(comp_func), m_is_top(true), m_color(color::BLACK) {}

template <class T>
RBNode<T>::~RBNode() {}

template <class T>
void RBNode<T>::recolor(color new_color) {
    if (m_is_top) {
        m_color = new_color;
    }
    throw NotImplemented("AAA");
}
