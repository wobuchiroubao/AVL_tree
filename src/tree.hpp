#pragma once

#include <iostream>
#include <cassert>

namespace SearchTrees {

  template <typename KeyT>
  struct AVL_Node {
    KeyT key_;
    AVL_Node *parent_ = nullptr, *left_ = nullptr, *right_ = nullptr;
    int height_ = 0;

    explicit AVL_Node(const KeyT &key, AVL_Node *parent = nullptr, int height = 0) : key_(key), parent_(parent), height_(height) {}
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
      return node->right_->height_ - node->left_->height_;
    }

    // iterator rotate_left(iterator sub_root, iterator right_child) {}
    // iterator rotate_right(iterator sub_root, iterator left_child) {}
    // iterator rotate_left_right() {}
    // iterator rotate_right_left() {}

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
        [](iterator it, size_t depth) {
          std::cout << std::string(depth, '\t');
          if (it->parent_) {
            if (it->parent_->left_ == it)
              std::cout << "L: ";
            else
              std::cout << "R: ";
          }
          std::cout << "(" << it->key_ << "; " << it->height_ << ")" << std::endl;
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