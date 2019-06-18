
#ifndef RBTREE_HPP
#define RBTREE_HPP

inline direction flip(direction orig) {
    if (orig == direction::LEFT) {
        return direction::RIGHT;
    }
    return direction::LEFT;
}

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
bool RBTree<T>::RBNode::isNil() const {
    return !m_key;
}

template <class T>
void RBTree<T>::RBNode::insert(T key) {
    if (!m_key) {
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
std::shared_ptr<typename RBTree<T>::RBNode> RBTree<T>::RBNode::kill() {
     std::shared_ptr<RBNode> retval = succ();
    auto killed_node = m_self.lock();
    if (m_r->m_key && m_l->m_key) {
        killed_node = retval; // retval is the successor
        retval = m_self.lock();
        m_key = std::move(killed_node->m_key);
    }
    auto replacing_node = killed_node->m_l->m_key ? killed_node->m_l : killed_node->m_r;
    auto killed_node_parent = killed_node->m_p.lock();
    if (!killed_node_parent) {
        // killed node is root
        m_tree.lock()->m_root = replacing_node;
    }
    else {
        auto killed_node_dir = killed_node->getDirectionFromParent();
        if (killed_node_dir == direction::LEFT) {
            killed_node_parent->m_l = replacing_node;
        }
        else {
            killed_node_parent->m_r = replacing_node;
        }
    }
    replacing_node->m_p = killed_node_parent;
    if (killed_node->m_color == color::BLACK) {
        replacing_node->blacken();
    }
    return retval;
}

template <class T>
std::shared_ptr<typename RBTree<T>::RBNode> RBTree<T>::RBNode::succ() const {
    return scan(direction::RIGHT);
}

template <class T>
std::shared_ptr<typename RBTree<T>::RBNode> RBTree<T>::RBNode::pred() const {
    return scan(direction::LEFT);
}

template <class T>
std::shared_ptr<typename RBTree<T>::RBNode> RBTree<T>::RBNode::scan(direction dir) const {
    auto ptr = getChild(dir);
    if (ptr && ptr->m_key) {
        // There is a child in the scanned direction, therefore successor is in subtree
        while(ptr->m_key) {
            // progress as much as possible in opposite direction in subtree
            ptr = ptr->getChild(flip(dir));
        }
        // Now we reached a leaf without a value, returning direct parent
        return ptr->m_p.lock();
    }
    ptr = m_self.lock();
    while (ptr->m_p.lock() && ptr->getDirectionFromParent() == dir) {
        ptr = ptr->m_p.lock();
    }
    if (!ptr) {
        return ptr;
    }
    // return either the direct parent, or if this is the last node - a nullptr
    return ptr->m_p.lock();
}

template <class T>
std::shared_ptr<typename RBTree<T>::RBNode> RBTree<T>::RBNode::minimum() const {
    if (!m_l || !m_l->m_key) {
        return m_self.lock();
    }
    return m_l->minimum();
}

template <class T>
std::shared_ptr<typename RBTree<T>::RBNode> RBTree<T>::RBNode::getChild(direction dir) const {
    if (dir == direction::LEFT) {
        return m_l;
    }
    return m_r;
}

template <class T>
direction RBTree<T>::RBNode::getDirectionFromParent() const {
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
    // Implementing as recursion is easier in this case
    // as we get free updates of `this`
    auto p = m_p.lock();
    if (!p) {
        // root, update red height by one
        m_color = color::BLACK;
        return;
    }
    
    m_color = color::RED;
    if (p->m_color == color::BLACK) {
        // All ok, can finish recursion
        return;
    }

    auto grandpa = p->m_p.lock();
    auto parent_dir = p->getDirectionFromParent();
    auto uncle = grandpa->getChild(flip(parent_dir));

    // Case 1
    if (uncle->m_color == color::RED) {
        // Red uncle, color father and uncle black and pass reddening upwards
        p->m_color = color::BLACK;
        uncle->m_color = color::BLACK;
        grandpa->redden();
        return;
    }
    
    auto dir = getDirectionFromParent();
        
    // Case 2
    if (dir != parent_dir) {
        // Rotate into case 3
        p->rotate(parent_dir);
        p->redden();
        return;
    }

    // Case 3
    p->m_color = color::BLACK;
    grandpa->m_color = color::RED;
    grandpa->rotate(flip(parent_dir));
}


template <class T>
void RBTree<T>::RBNode::blacken() {
    // More natural to implement using recursion
    auto p = m_p.lock();
    if (!p) {
        // Node is root, decrease black height by one
        return;
    }
    if (m_color == color::RED) {
        // Blackening a red node is easy
        m_color = color::BLACK;
        return;
    }
    auto dir = getDirectionFromParent();
    auto brother = p->getChild(flip(dir));
    // Case 1
    if (brother->m_color == color::RED) {
        brother->m_color = color::BLACK;
        p->m_color = color::RED;
        p->rotate(dir);
        // Move to black brother case
        blacken();
        return;
    }
    // Black brother
    // Case 2
    if (brother->m_l->m_color == color::BLACK && brother->m_r->m_color == color::BLACK) {
        // Both brother's kids are black
        // color brother red, and move extra black upwards
        brother->m_color = color::RED;
        p->blacken();
        return;
    }
    // At least one kid of brother is red
    // Case 3
    if (brother->getChild(flip(dir))->m_color == color::BLACK) {
        // Color same sided cousin in black and brother in red
        // then rotate brother into opposing side cousin (red -> case 4)
        brother->getChild(dir)->m_color = color::BLACK;
        brother->m_color = color::RED;
        brother->rotate(flip(dir));
        blacken();
        return;
    }
    // The remaining case is red opposite sided cousin
    // Case 4
    // Switch colors between (black) brother and parent, rotating parent and
    // adding a black node on both sides of new parent, rebalancing the tree
    brother->m_color = p->m_color;
    p->m_color = color::BLACK;
    brother->getChild(flip(dir))->m_color = color::BLACK;
    p->rotate(dir);
}

template <class T>
void RBTree<T>::RBNode::rotate(direction dir) {
    auto new_parent = getChild(flip(dir));
    if (!new_parent || !new_parent->m_key) {
        throw std::runtime_error("Attempting to rotate a NIL into node!");
    }

    auto old_parent = m_p.lock();
    auto self = m_self.lock();
    direction my_dir = direction::LEFT;
    if (old_parent) {
        my_dir = getDirectionFromParent();
    }

    m_p = new_parent;
    new_parent->m_p = old_parent;
    if (dir == direction::LEFT) {
        m_r = new_parent->m_l;
        m_r->m_p = self;
        new_parent->m_l = self;
    }
    else {
        m_l = new_parent->m_r;
        m_l->m_p = self;
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
const T& RBTree<T>::RBNode::get() const {
    return *m_key;
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
std::shared_ptr<typename RBTree<T>::RBNode> RBTree<T>::minimum() {
    return m_root->minimum();
}

#endif
