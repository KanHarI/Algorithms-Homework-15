
#include <memory>
#include <exception>


using std::weak_ptr;
using std::shared_ptr;

class keyAlreadyExists : public std::runtime_error {};

enum class color
{
	RED,
	BLACK
};

template <class T>
class RBNode final {
public:
	RBNode();
	~RBNode();
	
}

template <class T>
class RBTree final {
public:
	RBTree();
	~RBTree();

	// Operations
	insert(const T& key);
	delete(const T& key);
private:
	color m_color;
	weak_ptr<RBTree<T>> m_p; // parent. Weak ptr to prevent memory leak upon deletion
	shared_ptr<RBTree<T>> m_l; // left child
	shared_ptr<RBTree<T>> m_r; // right child
	T m_key;
};

template <class T>
RBTree<T>::RBTree(const T& rootKey) : m_key(rootKey), m_color(color::BLACK) {}

template <class T>
RBTree<T>::~RBTree() {} // shared & weak ptrs do the magic for us

template <class T>
RBTree<T>::insert(const T& key) {

}
