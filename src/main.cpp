#include <iostream>
#include "tree.hpp"

using SearchTrees::AVL_Tree;

int main() {
  AVL_Tree<int> avl{};
  avl.insert(10);
  std::cout << "State after insert(10):" << std::endl;
  avl.print();
  avl.insert(8);
  std::cout << "State after insert(8):" << std::endl;
  avl.print();
  avl.insert(9);
  std::cout << "State after insert(9):" << std::endl;
  avl.print();
  avl.insert(4);
  std::cout << "State after insert(4):" << std::endl;
  avl.print();
  avl.insert(3);
  std::cout << "State after insert(3):" << std::endl;
  avl.print();
  avl.insert(6);
  std::cout << "State after insert(6):" << std::endl;
  avl.print();
  avl.insert(5);
  std::cout << "State after insert(5):" << std::endl;
  avl.print();
  avl.insert(13);
  std::cout << "State after insert(13):" << std::endl;
  avl.print();
  avl.insert(16);
  std::cout << "State after insert(16):" << std::endl;
  avl.print();
  avl.insert(14);
  std::cout << "State after insert(14):" << std::endl;
  avl.print();

  // avl.print();
  // std::cout << "lower bound(2) = " << avl.lower_bound(2)->key_ << std::endl;
  // std::cout << "lower bound(5) = " << avl.lower_bound(5)->key_ << std::endl;
  // std::cout << "lower bound(11) = " << avl.lower_bound(11)->key_ << std::endl;
  // std::cout << "lower bound(12) = " << avl.lower_bound(12)->key_ << std::endl;

  // std::cout << "upper bound(2) = " << avl.upper_bound(2)->key_ << std::endl;
  // std::cout << "upper bound(5) = " << avl.upper_bound(5)->key_ << std::endl;
  // std::cout << "upper bound(11) = " << avl.upper_bound(11)->key_ << std::endl;
  // std::cout << "upper bound(12) = " << avl.upper_bound(12)->key_ << std::endl;

  // avl.erase(12);
  // avl.erase(13);
  // avl.erase(8);

  // avl.print();

  return 0;
}