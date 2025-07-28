#pragma once

#include <iostream>
#include <cassert>

namespace SearchTrees {

template <typename KeyT>
struct BST_Node {
  KeyT key_;
  BST_Node *parent_ = nullptr, *left_ = nullptr, *right_ = nullptr;

  explicit BST_Node(const KeyT &key, BST_Node *parent = nullptr)
    : key_(key)
    , parent_(parent) {}
  virtual ~BST_Node() = default;
  BST_Node(const BST_Node &other) = delete;
  BST_Node(BST_Node &&other) = delete;
  BST_Node& operator= (const BST_Node &rhs) = delete;
  BST_Node& operator= (BST_Node &&rhs) = delete;
  virtual BST_Node *clone() const {
    return new BST_Node<KeyT>{key_};
  }
};


template <typename KeyT>
struct AVL_Node final : public BST_Node<KeyT> {
  int height_ = 1;

  explicit AVL_Node(const KeyT &key, AVL_Node *parent = nullptr, int height = 1)
    : BST_Node<KeyT>{key, parent}
    , height_(height) {}
  AVL_Node(const AVL_Node &other) = delete;
  AVL_Node(AVL_Node &&other) = delete;
  AVL_Node& operator= (const AVL_Node &rhs) = delete;
  AVL_Node& operator= (AVL_Node &&rhs) = delete;
  ~AVL_Node() = default;
  AVL_Node *clone() const override {
    return new AVL_Node<KeyT>{this->key_, nullptr, height_};
  }
};


template <typename KeyT>
class BST_Tree {
  using bst_iterator = BST_Node<KeyT> *;
  using bst_const_iterator = const BST_Node<KeyT> *;
protected:
  bst_iterator root_ = nullptr;

protected: // traversal
  enum class visited_child_t : char { NONE, LEFT, RIGHT };
  enum class order_t : char { PRE, POST };

  template <typename Func>
  void depth_traversal(bst_iterator root, order_t order, Func func) {
    visited_child_t visited = visited_child_t::NONE;
    size_t depth = 0;

    for (auto it = root; it != nullptr;) {
      if (order == order_t::PRE && visited == visited_child_t::NONE)
        func(it, depth);
      if (it->left_ && visited == visited_child_t::NONE) {
        it = it->left_;
        ++depth;
      } else if (it->right_ && visited != visited_child_t::RIGHT) {
        it = it->right_;
        visited = visited_child_t::NONE;
        ++depth;
      } else {
        bst_iterator parent = it != root ? it->parent_ : nullptr;
        if (parent) {
          assert(parent->left_ == it || parent->right_ == it);
          if (parent->left_ == it) {
            visited = visited_child_t::LEFT;
          } else {
            visited = visited_child_t::RIGHT;
          }
          --depth;
        }
        bst_iterator tmp = it;
        it = parent;
        if (order == order_t::POST)
          func(tmp, depth);
      }
    }
  }

  bst_iterator copy_depth_traversal(bst_iterator root) {
    if (!root)
      return nullptr;
    bst_iterator copy_root = root->clone();

    for (auto it = root, copy_it = copy_root;;) {
      if (it->left_ && !copy_it->left_) {
        it = it->left_;
        bst_iterator parent = copy_it;
        copy_it = it->clone();
        copy_it->parent_ = parent;
        copy_it->parent_->left_ = copy_it;
      } else if (it->right_ && !copy_it->right_) {
        it = it->right_;
        bst_iterator parent = copy_it;
        copy_it = it->clone();
        copy_it->parent_ = parent;
        copy_it->parent_->right_ = copy_it;
      } else if (it != root) {
        it = it->parent_;
        copy_it = copy_it->parent_;
      } else {
        break;
      }
    }
    return copy_root;
  }

public: // ctors & dtors
  BST_Tree() noexcept {}
  virtual ~BST_Tree() {
    depth_traversal(
      root_,
      order_t::POST,
      [](bst_iterator it, size_t _) {
        delete it;
      }
    );
  }
  BST_Tree(const BST_Tree &other) : root_(copy_depth_traversal(other.root_)) {}
  BST_Tree(BST_Tree &&other) noexcept : root_(other.root_) { other.root_ = nullptr; }
  virtual BST_Tree& operator= (const BST_Tree &rhs) {
    if (this == &rhs)
      return *this;

    clear();
    root_ = copy_depth_traversal(rhs.root_);
    return *this;
  }
  virtual BST_Tree& operator= (BST_Tree &&rhs) noexcept {
    if (this == &rhs)
      return *this;

    std::swap(root_, rhs.root_);
    return *this;
  }

public: // selectors
  virtual bst_const_iterator root() const & { return root_; }
  virtual bst_iterator root() & { return root_; }
  virtual bst_const_iterator end() const & noexcept { return nullptr; }
  virtual bst_iterator end() & noexcept { return nullptr; }
  bool empty() const noexcept { return !root_; }
  bool contains(KeyT key) const {
    bst_iterator node = find(key);
    return node != end();
  }
  virtual bst_const_iterator find(KeyT key) const & {
    bst_const_iterator lb = lower_bound(key);
    return (lb && lb->key_ == key) ? lb : end();
  }
  virtual bst_iterator find(KeyT key) & {
    return const_cast<bst_iterator>(const_cast<const BST_Tree*>(this)->find(key));
  }

  virtual bst_const_iterator lower_bound(KeyT key, bst_const_iterator root) const & {
    if (empty())
      return end();

    bst_const_iterator cur_min = end();
    for (auto it = root;;) {
      if (it->key_ < key) {
        if (!it->right_)
          return cur_min;
        it = it->right_;
      } else if (key < it->key_) {
        cur_min = it;
        if (!it->left_)
          return cur_min;
        it = it->left_;
      } else { // key == it->key_
        return it;
      }
    }
  }

  virtual bst_const_iterator lower_bound(KeyT key) const & { return lower_bound(key, root_); }

  virtual bst_iterator lower_bound(KeyT key, bst_iterator root) & {
    return const_cast<bst_iterator>(const_cast<const BST_Tree*>(this)->lower_bound(key, root));
  }

  virtual bst_iterator lower_bound(KeyT key) & { return lower_bound(key, root_);}

  virtual bst_const_iterator upper_bound(KeyT key, bst_const_iterator root) const & {
    if (empty())
      return end();

    bst_const_iterator cur_min = end();
    for (auto it = root;;) {
      if (it->key_ < key || it->key_ == key) {
        if (!it->right_)
          return cur_min;
        it = it->right_;
      } else {
        cur_min = it;
        if (!it->left_)
          return cur_min;
        it = it->left_;
      }
    }
  }

  virtual bst_const_iterator upper_bound(KeyT key) const & { return upper_bound(key, root_); }

  virtual bst_iterator upper_bound(KeyT key, bst_iterator root) & {
    return const_cast<bst_iterator>(const_cast<const BST_Tree*>(this)->upper_bound(key, root));
  }

  virtual bst_iterator upper_bound(KeyT key) & { return upper_bound(key, root_); }

  virtual void dump(std::ostream& os) {
    depth_traversal(
      root_,
      order_t::PRE,
      [this, &os](bst_iterator it, size_t depth) {
        os << std::string(depth, '\t');
        if (it->parent_) {
          if (it->parent_->left_ == it)
            os << "L: ";
          else
            os << "R: ";
        }
        os << "(" << it->key_ << ")\n";
      }
    );
  }

public: // modifiers
  void clear() {
    depth_traversal(
      root_,
      order_t::POST,
      [](bst_iterator it, size_t _) {
        delete it;
      }
    );
    root_ = end();
  }

  virtual bst_iterator create_node(KeyT key) const {
    return new BST_Node<KeyT>{key};
  }

  virtual bst_iterator insert(KeyT key) & {
    if (!root_) {
      root_ = create_node(key);
      return root_;
    }

    bst_iterator lb = lower_bound(key);
    if (lb && lb->key_ == key)
      return lb;

    bst_iterator new_node = create_node(key);
    if (!lb) {
      for (auto it = root_;; it = it->right_) {
        if (!it->right_) {
          new_node->parent_ = it;
          it->right_ = new_node;
          break;
        }
      }
    } else if (!lb->left_) {
      new_node->parent_ = lb;
      lb->left_ = new_node;
    } else {
      for (auto it = lb->left_;; it = it->right_) {
        if (!it->right_) {
          new_node->parent_ = it;
          it->right_ = new_node;
          break;
        }
      }
    }

    return new_node;
  }

  virtual bool erase(KeyT key) & {
    bst_iterator node = find(key);
    if (node == end())
      return false;

    bst_iterator successor = node->left_ ? node->left_ : node->right_;
    if (node->left_ && node->right_) { // 2 children
      successor = upper_bound(node->key_, node->right_);
      assert(!successor->left_);
      successor->left_ = node->left_;
      node->left_->parent_ = successor;

      // place successor in root of (node->right_) subtree
      if (successor->parent_ != node) {
        assert(successor->parent_->left_ == successor);
        successor->parent_->left_ = successor->right_;
        if (successor->right_) {
          successor->right_->parent_ = successor->parent_;
        }
        successor->right_ = node->right_;
        node->right_->parent_ = successor;
      }
    }

    // move successor on node's place
    assert(!node->parent_ || node->parent_->left_ == node || node->parent_->right_ == node);
    if (!node->parent_) {
      assert(root_ == node);
      root_ = successor;
    } else if (node->parent_->left_ == node) {
      node->parent_->left_ = successor;
    } else {
      node->parent_->right_ = successor;
    }
    if (successor) {
      successor->parent_ = node->parent_;
    }

    delete node;

    return true;
  }
};


template <typename KeyT>
std::ostream& operator<< (std::ostream& os, BST_Tree<KeyT>& tree) {
  tree.dump(os);
  return os;
}


template <typename KeyT>
class AVL_Tree final : public BST_Tree<KeyT> {
  using bst_iterator = BST_Node<KeyT> *;
  using bst_const_iterator = const BST_Node<KeyT> *;
  using avl_iterator = AVL_Node<KeyT> *;
  using avl_const_iterator = const AVL_Node<KeyT> *;

public: // ctors & dtors
  AVL_Tree() noexcept : BST_Tree<KeyT>{} {}
  AVL_Tree(const AVL_Tree &other) : BST_Tree<KeyT>{other} {}
  AVL_Tree(AVL_Tree &&other) noexcept : BST_Tree<KeyT>{std::move(other)} {}

private: // rotations
  int calc_height(avl_iterator node) const {
    return std::max(
      (node->left_ ? static_cast<avl_iterator>(node->left_)->height_ : 0),
      (node->right_ ? static_cast<avl_iterator>(node->right_)->height_ : 0)
    ) + 1;
  }
  int calc_balance_factor(avl_iterator node) const {
    return (
      (node->right_ ? static_cast<avl_iterator>(node->right_)->height_ : 0)
      - (node->left_ ? static_cast<avl_iterator>(node->left_)->height_ : 0));
  }

  void simple_rotate_swap_parent(bst_iterator sub_root, bst_iterator child) {
    bst_iterator &sub_root_parent = sub_root->parent_;
    if (sub_root_parent) {
      assert(sub_root_parent->left_ == sub_root || sub_root_parent->right_ == sub_root);
      if (sub_root_parent->left_ == sub_root) {
        sub_root_parent->left_ = child;
      } else {
        sub_root_parent->right_ = child;
      }
      child->parent_ = sub_root_parent;
    } else {
      assert(this->root_ == sub_root);
      this->root_ = child;
      child->parent_ = nullptr;
    }
  }

  avl_iterator rotate_left(avl_iterator sub_root, avl_iterator right_child) & {
    assert(sub_root->right_ == right_child && right_child->parent_ == sub_root);

    simple_rotate_swap_parent(sub_root, right_child);

    sub_root->right_ = right_child->left_;
    if (sub_root->right_)
      sub_root->right_->parent_ = sub_root;
    right_child->left_ = sub_root;
    sub_root->parent_ = right_child;

    sub_root->height_ = calc_height(sub_root);
    right_child->height_ = calc_height(right_child);

    return right_child;
  }

  avl_iterator rotate_right(avl_iterator sub_root, avl_iterator left_child) & {
    assert(sub_root->left_ == left_child && left_child->parent_ == sub_root);

    simple_rotate_swap_parent(sub_root, left_child);

    sub_root->left_ = left_child->right_;
    if (sub_root->left_)
      sub_root->left_->parent_ = sub_root;
    left_child->right_ = sub_root;
    sub_root->parent_ = left_child;

    sub_root->height_ = calc_height(sub_root);
    left_child->height_ = calc_height(left_child);

    return left_child;
  }

  avl_iterator rotate_left_right(avl_iterator sub_root, avl_iterator left_child) & {
    assert(left_child->right_);
    avl_iterator new_child = rotate_left(left_child, static_cast<avl_iterator>(left_child->right_));
    rotate_right(sub_root, new_child);
    return new_child;
  }

  avl_iterator rotate_right_left(avl_iterator sub_root, avl_iterator right_child) & {
    assert(right_child->left_);
    avl_iterator new_child = rotate_right(right_child, static_cast<avl_iterator>(right_child->left_));
    rotate_left(sub_root, new_child);
    return new_child;
  }

  template <typename Func>
  void retrace(bst_iterator start, Func break_cond) {
    for (
      auto node = static_cast<avl_iterator>(start);
      node != nullptr;
      node = static_cast<avl_iterator>(node->parent_)
    ) {
      node->height_ = calc_height(node);
      int bf = calc_balance_factor(node);

      if (std::abs(bf) == 2) {
        avl_iterator child = static_cast<avl_iterator>((bf < 0) ? node->left_ : node->right_);
        assert(child);
        int ch_bf = calc_balance_factor(child);

        if (break_cond(bf)) {
          break;
        } else if (bf == -2) { // left heavy
          if (ch_bf <= 0) { // ch left heavy or balanced
            rotate_right(node, child);
          } else { // ch right heavy
            rotate_left_right(node, child);
          }
        } else if (bf == 2) { // right heavy
          if (ch_bf >= 0) { // ch right heavy or balanced
            rotate_left(node, child);
          } else { // ch left heavy
            rotate_right_left(node, child);
          }
        }
      }
    }
  }

public: // selectors
  avl_const_iterator root() const & override { return static_cast<avl_iterator>(this->root_); }
  avl_iterator root() & override { return static_cast<avl_iterator>(this->root_); }
  avl_const_iterator end() const & noexcept override { return nullptr; }
  avl_iterator end() & noexcept override { return nullptr; }
  avl_const_iterator find(KeyT key) const & override {
    return static_cast<avl_const_iterator>(BST_Tree<KeyT>::find(key));
  }
  avl_iterator find(KeyT key) & override {
    return const_cast<avl_iterator>(const_cast<const AVL_Tree*>(this)->find(key));
  }

  avl_const_iterator lower_bound(KeyT key, bst_const_iterator root) const & override {
      return static_cast<avl_const_iterator>(BST_Tree<KeyT>::lower_bound(key, root));
  }

  avl_const_iterator lower_bound(KeyT key) const & override { return lower_bound(key, this->root_); }

  avl_iterator lower_bound(KeyT key, bst_iterator root) & override {
    return const_cast<avl_iterator>(const_cast<const AVL_Tree*>(this)->lower_bound(key, root));
  }

  avl_iterator lower_bound(KeyT key) & override { return lower_bound(key, this->root_);}

  avl_const_iterator upper_bound(KeyT key, bst_const_iterator root) const & override {
    return static_cast<avl_const_iterator>(BST_Tree<KeyT>::upper_bound(key, root));
  }

  avl_const_iterator upper_bound(KeyT key) const & override { return upper_bound(key, this->root_); }

  avl_iterator upper_bound(KeyT key, bst_iterator root) & override {
    return const_cast<avl_iterator>(const_cast<const AVL_Tree*>(this)->upper_bound(key, root));
  }

  avl_iterator upper_bound(KeyT key) & override { return upper_bound(key, this->root_); }

  void dump(std::ostream& os) override {
    this->depth_traversal(
      this->root_,
      BST_Tree<KeyT>::order_t::PRE,
      [this, &os](bst_iterator it, size_t depth) {
        os << std::string(depth, '\t');
        if (it->parent_) {
          if (it->parent_->left_ == it)
          os << "L: ";
          else
          os << "R: ";
        }
        auto avl_it = static_cast<avl_iterator>(it);
        os << "(" << avl_it->key_ << "; " << avl_it->height_ << "; " << calc_balance_factor(avl_it) << ")\n";
      }
    );
  }

  avl_iterator create_node(KeyT key) const override {
    return new AVL_Node<KeyT>{key};
  }

  avl_iterator insert(KeyT key) & override {
    avl_iterator new_node = static_cast<avl_iterator>(BST_Tree<KeyT>::insert(key));
    retrace(
      static_cast<avl_iterator>(new_node->parent_),
      [](int bf) { return (bf == 0); }
    );

    return new_node;
  }

  bool erase(KeyT key) & override {
    bst_iterator node = find(key);
    if (node == end())
      return false;

    bst_iterator successor = node->left_ ? node->left_ : node->right_;
    if (node->left_ && node->right_) { // 2 children
      successor = upper_bound(node->key_, node->right_);
      assert(!successor->left_);
      successor->left_ = node->left_;
      node->left_->parent_ = successor;

      // place successor in root of (node->right_) subtree
      if (successor->parent_ != node) {
        assert(successor->parent_->left_ == successor);
        successor->parent_->left_ = successor->right_;
        if (successor->right_) {
          successor->right_->parent_ = successor->parent_;
        }
        successor->right_ = node->right_;
        node->right_->parent_ = successor;
      }
    }

    // calc retrace start point
    bst_iterator retrase_start = nullptr;
    if (!node->left_ || !node->right_) { // no children or 1 child
      retrase_start = node->parent_;
    } else { // 2 children
      if (successor->parent_ == node) {
        retrase_start = successor;
      } else {
        retrase_start = successor->parent_;
      }
    }

    // move successor on node's place
    assert(!node->parent_ || node->parent_->left_ == node || node->parent_->right_ == node);
    if (!node->parent_) {
      assert(this->root_ == node);
      this->root_ = successor;
    } else if (node->parent_->left_ == node) {
      node->parent_->left_ = successor;
    } else {
      node->parent_->right_ = successor;
    }
    if (successor) {
      successor->parent_ = node->parent_;
    }

    delete node;
    retrace(
      static_cast<avl_iterator>(retrase_start),
      [](int bf) { return (std::abs(bf) == 1); }
    );

    return true;
  }
};

} // SearchTrees