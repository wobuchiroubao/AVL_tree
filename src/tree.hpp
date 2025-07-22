#pragma once

#include <iostream>
#include <cassert>

namespace SearchTrees {

  template <typename KeyT>
  struct AVL_Node {
    KeyT key_;
    AVL_Node *parent_ = nullptr, *left_ = nullptr, *right_ = nullptr;
    int height_ = 1;

    explicit AVL_Node(const KeyT &key, AVL_Node *parent = nullptr, int height = 1) : key_(key), parent_(parent), height_(height) {}
  };

  template <typename KeyT>
  class AVL_Tree {
    using iterator = AVL_Node<KeyT> *;
    using const_iterator = const AVL_Node<KeyT> *;
    iterator root_ = nullptr;

  private: // traversal
    enum visited_children_t { NONE, LEFT, RIGHT };
    enum order_t { PRE, POST };

    template <typename Func>
    void depth_traversal(iterator root, order_t order, Func func) {
      visited_children_t visited = NONE;
      size_t depth = 0;

      for (auto it = root; it != nullptr;) {
        if (order == PRE && visited == NONE)
          func(it, depth);
        if (it->left_ && visited == NONE) {
          it = it->left_;
          ++depth;
        } else if (it->right_ && visited != RIGHT) {
          it = it->right_;
          visited = NONE;
          ++depth;
        } else {
          iterator parent = it != root ? it->parent_ : nullptr;
          if (parent) {
            assert(parent->left_ == it || parent->right_ == it);
            if (parent->left_ == it) {
              visited = LEFT;
            } else {
              visited = RIGHT;
            }
            --depth;
          }
          iterator tmp = it;
          it = parent;
          if (order == POST)
            func(tmp, depth);
        }
      }
    }

    iterator copy_depth_traversal(iterator root) {
      if (!root)
        return nullptr;
      iterator copy_root = new AVL_Node<KeyT>{root->key_, nullptr, root->height_};

      for (auto it = root, copy_it = copy_root;;) {
        if (it->left_ && !copy_it->left_) {
          it = it->left_;
          copy_it = new AVL_Node<KeyT>{it->key_, copy_it, it->height_};
          copy_it->parent_->left_ = copy_it;
        } else if (it->right_ && !copy_it->right_) {
          it = it->right_;
          copy_it = new AVL_Node<KeyT>{it->key_, copy_it, it->height_};
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
    AVL_Tree() {}
    AVL_Tree(const AVL_Tree &other) : root_(copy_depth_traversal(other.root_)) {}
    AVL_Tree(AVL_Tree &&other) : root_(other.root_) { other.root_ = nullptr; }
    ~AVL_Tree() { clear(); }
    AVL_Tree& operator= (const AVL_Tree &rhs) {
      if (this == &rhs)
        return *this;

      clear();
      root_ = copy_depth_traversal(rhs.root_);
      return *this;
    }
    AVL_Tree& operator= (AVL_Tree &&rhs) {
      if (this == &rhs)
        return *this;

      std::swap(root_, rhs.root_);
      return *this;
    }

  private: // rotations
    int calc_height(iterator node) const {
      return std::max(
        (node->left_ ? node->left_->height_ : 0),
        (node->right_ ? node->right_->height_ : 0)
      ) + 1;
    }
    int calc_balance_factor(iterator node) const {
      return (
        (node->right_ ? node->right_->height_ : 0)
        - (node->left_ ? node->left_->height_ : 0));
    }

    void simple_rotate_swap_parent(iterator sub_root, iterator child) {
      iterator &sub_root_parent = sub_root->parent_;
      if (sub_root_parent) {
        assert(sub_root_parent->left_ == sub_root || sub_root_parent->right_ == sub_root);
        if (sub_root_parent->left_ == sub_root) {
          sub_root_parent->left_ = child;
        } else {
          sub_root_parent->right_ = child;
        }
        child->parent_ = sub_root_parent;
      } else {
        assert(root_ == sub_root);
        root_ = child;
        child->parent_ = nullptr;
      }
    }

    iterator rotate_left(iterator sub_root, iterator right_child) & {
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

    iterator rotate_right(iterator sub_root, iterator left_child) & {
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

    iterator rotate_left_right(iterator sub_root, iterator left_child) & {
      assert(left_child->right_);
      iterator new_child = rotate_left(left_child, left_child->right_);
      rotate_right(sub_root, new_child);
      return new_child;
    }

    iterator rotate_right_left(iterator sub_root, iterator right_child) & {
      assert(right_child->left_);
      iterator new_child = rotate_right(right_child, right_child->left_);
      rotate_left(sub_root, new_child);
      return new_child;
    }

    template <typename Func>
    void retrace(iterator start, Func break_cond) {
      for (auto node = start; node != nullptr; node = node->parent_) {
        node->height_ = calc_height(node);
        int bf = calc_balance_factor(node);

        if (std::abs(bf) == 2) {
          iterator child = (bf < 0) ? node->left_ : node->right_;
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
    const_iterator root() const & { return root_; }
    iterator root() & { return root_; }
    const_iterator end() const & { return nullptr; }
    iterator end() & { return nullptr; }
    bool empty() const { return !root_; }
    bool contains(KeyT key) const {
      iterator node = find(key);
      return node != end();
    }
    const_iterator find(KeyT key) const & {
      const_iterator lb = lower_bound(key);
      return (lb && lb->key_ == key) ? lb : end();
    }
    iterator find(KeyT key) & {
      return const_cast<iterator>(const_cast<const AVL_Tree*>(this)->find(key));
    }

    const_iterator lower_bound(KeyT key, const_iterator root) const & {
      if (empty())
        return end();

      const_iterator cur_min = end();
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

    const_iterator lower_bound(KeyT key) const & { return lower_bound(key, root_); }

    iterator lower_bound(KeyT key, iterator root) & {
      return const_cast<iterator>(const_cast<const AVL_Tree*>(this)->lower_bound(key, root));
    }

    iterator lower_bound(KeyT key) & { return lower_bound(key, root_);}

    const_iterator upper_bound(KeyT key, const_iterator root) const & {
      if (empty())
        return end();

      const_iterator cur_min = end();
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

    const_iterator upper_bound(KeyT key) const & { return upper_bound(key, root_); }

    iterator upper_bound(KeyT key, iterator root) & {
      return const_cast<iterator>(const_cast<const AVL_Tree*>(this)->upper_bound(key, root));
    }

    iterator upper_bound(KeyT key) & { return upper_bound(key, root_); }

    void dump(std::ostream& os) {
      depth_traversal(
        root_,
        PRE,
        [this, &os](iterator it, size_t depth) {
          os << std::string(depth, '\t');
          if (it->parent_) {
            if (it->parent_->left_ == it)
              os << "L: ";
            else
              os << "R: ";
          }
          os << "(" << it->key_ << "; " << it->height_ << "; " << calc_balance_factor(it) << ")\n";
        }
      );
    }

  public: // modifiers
    void clear() {
      depth_traversal(
        root_,
        POST,
        [](iterator it, size_t _) {
          delete it;
        }
      );
      root_ = end();
    }

    iterator insert(KeyT key) & {
      if (!root_) {
        root_ = new AVL_Node<KeyT>{key};
        return root_;
      }

      iterator lb = lower_bound(key);
      if (lb && lb->key_ == key)
        return lb;

      iterator new_node = new AVL_Node<KeyT>{key};
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
      retrace(
        new_node->parent_,
        [](int bf) { return (bf == 0); }
      );

      return new_node;
    }

    bool erase(KeyT key) & {
      iterator node = find(key);
      if (node == end())
        return false;

      iterator successor = node->left_ ? node->left_ : node->right_;
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
      iterator retrase_start = nullptr;
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
      retrace(
        retrase_start,
        [](int bf) { return (std::abs(bf) == 1); }
      );

      return true;
    }
  };

  template <typename KeyT>
  std::ostream& operator<<(std::ostream& os, AVL_Tree<KeyT>& tree) {
    tree.dump(os);
    return os;
  }

} // SearchTrees