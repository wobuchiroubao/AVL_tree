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
    void depth_traversal(order_t order, Func func) {
      visited_children_t visited = NONE;
      size_t depth = 0;

      for (auto it = root_; it != nullptr;) {
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
          if (it->parent_) {
            assert(it->parent_->left_ == it || it->parent_->right_ == it);
            if (it->parent_->left_ == it) {
              visited = LEFT;
            } else {
              visited = RIGHT;
            }
            --depth;
          }
          iterator tmp = it;
          it = it->parent_;
          if (order == POST)
            func(tmp, depth);
        }
      }
    }

  public: // ctors & dtors
    AVL_Tree() {}
    AVL_Tree(const AVL_Tree &other) = delete;
    ~AVL_Tree() {
      depth_traversal(
        POST,
        [](iterator it, size_t _) {
          delete it;
        }
      );
    }
    AVL_Tree& operator= (const AVL_Tree &rhs) = delete;

  private: // rotations
    int calc_height(iterator node) {
      return std::max(
        (node->left_ ? node->left_->height_ : 0),
        (node->right_ ? node->right_->height_ : 0)
      ) + 1;
    }
    int calc_balance_factor(iterator node) {
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

    iterator rotate_left(iterator sub_root, iterator right_child) {
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

    iterator rotate_right(iterator sub_root, iterator left_child) {
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

    iterator rotate_left_right(iterator sub_root, iterator left_child) {
      assert(left_child->right_);
      iterator new_child = rotate_left(left_child, left_child->right_);
      rotate_right(sub_root, new_child);
      return new_child;
    }

    iterator rotate_right_left(iterator sub_root, iterator right_child) {
      assert(right_child->left_);
      iterator new_child = rotate_right(right_child, right_child->left_);
      rotate_left(sub_root, new_child);
      return new_child;
    }

    void retrace(iterator start) {
      for (auto child = start; child->parent_ != nullptr; child = child->parent_) {
        iterator &parent = child->parent_;
        assert(parent->left_ == child || parent->right_ == child);
        parent->height_ = calc_height(parent);
        int par_bf = calc_balance_factor(parent);
        int ch_bf = calc_balance_factor(child);

        if (par_bf == 2) {
          if (ch_bf == 1) {
            rotate_left(parent, child);
          } else {
            rotate_right_left(parent, child);
          }
        } else if (par_bf == -2) {
          if (ch_bf == 1) {
            rotate_left_right(parent, child);
          } else {
            rotate_right(parent, child);
          }
        }
      }
    }

  public: // selectors
    iterator root() { return root_; }
    iterator end() { return nullptr; }
    bool empty() const { return !root_; }
    bool contains(KeyT key) {
      iterator node = find(key);
      return node != end();
    }
    iterator find(KeyT key) {
      iterator lb = lower_bound(key);
      return (lb && lb->key_ == key) ? lb : end();
    }
    iterator lower_bound(KeyT key) {
      if (empty())
        return end();

      iterator cur_min = end();
      for (auto it = root_;;) {
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
    iterator upper_bound(KeyT key) {
      if (empty())
        return end();

      iterator cur_min = end();
      for (auto it = root_;;) {
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
    // int distance(iterator begin, iterator end) const;

    void print() {
      depth_traversal(
        PRE,
        [this](iterator it, size_t depth) {
          std::cout << std::string(depth, '\t');
          if (it->parent_) {
            if (it->parent_->left_ == it)
              std::cout << "L: ";
            else
              std::cout << "R: ";
          }
          std::cout << "(" << it->key_ << "; " << it->height_ << "; " << calc_balance_factor(it) << ")" << std::endl;
        }
      );
    }

  public: // modifiers
    iterator insert(KeyT key) {
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
      retrace(new_node);

      return new_node;
    }

    bool erase(KeyT key) {
      iterator node = find(key);
      if (node == end())
        return false;

      if (node->left_ && node->right_) {
        for (auto it = node->left_;; it = it->right_) {
          if (!it->right_) {
            it->right_ = node->right_;
            node->right_->parent_ = it;
            break;
          }
        }
      }
      iterator new_node = node->left_ ? node->left_ : node->right_;
      if (new_node) {
        new_node->parent_ = node->parent_;
      }
      if (node->parent_) {
        assert(node->parent_->left_ == node || node->parent_->right_ == node);
        if (node->parent_->left_ == node) {
          node->parent_->left_ = new_node;
        } else {
          node->parent_->right_ = new_node;
        }
      } else {
        assert(root_ == node);
        root_ = new_node;
      }
      delete node;
      return true;
    }
  };

}