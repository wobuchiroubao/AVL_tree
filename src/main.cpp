#include <iostream>
#include "tree.hpp"

using SearchTrees::AVL_Tree;

int main() {
  AVL_Tree<int> avl{};
  avl.insert(10);
  avl.insert(8);
  avl.insert(9);
  avl.insert(7);
  avl.insert(6);
  avl.insert(13);
  avl.insert(11);

  avl.print();
  std::cout << "lower bound(5) = " << avl.lower_bound(5)->key_ << std::endl;
  std::cout << "lower bound(11) = " << avl.lower_bound(11)->key_ << std::endl;
  std::cout << "lower bound(12) = " << avl.lower_bound(12)->key_ << std::endl;

  return 0;
}