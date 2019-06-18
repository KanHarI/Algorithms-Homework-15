
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

template <class T>
class RBTree final {
private:
    // This struct keeps users from calling RBTree::RBTree.
    struct ctor_protector_t{};

    // Node in the balanced tree
    class RBNode;

public:
    // Type for key comparer function
    using comp_func_t = std::function<int(T, T)>;
    
    // Do not call this method - create a tree via the createTree function
    // instead. The nodes of the tree needs to have a pointer to it, however
    // as we are using smart pointers we need to create a shared_ptr to current
    // object. Nodes will hold it as weak_ptr
    RBTree(comp_func_t, ctor_protector_t);
    ~RBTree();

    // Creates an empty tree with comp_func as key comparison function
    static std::shared_ptr<RBTree<T>> createTree(comp_func_t comp_func);

    // Inserts a new key into the tree
    void insert(T key);

    // Removes a key from the tree
    void remove(T key);

    // Finds minimal node in tree
    std::shared_ptr<RBNode> minimum();

private:
    // Root of the tree
    std::shared_ptr<RBNode> m_root;

    // Key comparison function
    comp_func_t m_comp_func;
};

template <class T>
class RBTree<T>::RBNode final {
public:
    RBNode(std::weak_ptr<RBTree<T>> tree);
    ~RBNode();

    // Inserts a key to subtree under curent node
    void insert(T key);
    
    // Removes current node from tree. WARNING: you must hold a shared_ptr to
    // current node when calling this, otherwise node may be destroyed 
    // mid-execution when ref count reaches 0.
    // Returns succcessor node (usefull to allow iterating and deleting nodes)
    std::shared_ptr<RBNode> kill();

    // Find successor node
    std::shared_ptr<RBNode> succ() const;
    
    // Find predecessor node
    std::shared_ptr<RBNode> pred() const;
    
    // Find successor/predecessor node in given direction
    std::shared_ptr<RBNode> scan(direction dir) const;
    
    // Find minimal keyed node
    std::shared_ptr<RBNode> minimum() const;
    
    // Get child in chosen direction
    std::shared_ptr<RBNode> getChild(direction dir) const;
    
    // Finds whether this is a left or right child
    direction getDirectionFromParent() const;

    // True iff this is a Nil
    bool isNil() const;

    // Create a new node to be added to a tree
    static std::shared_ptr<RBNode> createNode(std::weak_ptr<RBTree<T>> tree);

    // Returns current node value
    const T& get() const;

    // Create child of current node
    std::shared_ptr<RBNode> createChild();

private:
    // Try to turn a node red rebalance tree
    void redden();

    // Try to add black height to current node
    void blacken();

    // Rotates tree
    void rotate(direction dir);

    color m_color;
    std::weak_ptr<RBTree<T>> m_tree; // pointer to tree object
    std::weak_ptr<RBNode> m_self; // weak ptr to self. Used to pass to children nodes
    std::weak_ptr<RBNode> m_p; // parent. Weak ptr to prevent memory leak upon deletion
    std::shared_ptr<RBNode> m_l; // left child
    std::shared_ptr<RBNode> m_r; // right child

    // A unique_ptr to current node's key. Used to check whether a node is Nil 
    // (nullptr iff node is Nil)
    std::unique_ptr<T> m_key;

};

// Implementation
#include "RBTree.hpp"

#endif
