#include <iostream>
#include "tree.hpp"

using SearchTrees::AVL_Tree;

int main() {
  AVL_Tree<int> avl{};
  avl.insert(11);
  avl.insert(8);
  avl.insert(9);
  avl.insert(4);
  avl.insert(3);
  avl.insert(6);
  avl.insert(5);
  avl.insert(13);
  avl.insert(16);
  avl.insert(14);
  avl.insert(10);
  avl.print();

  avl.erase(12);
  std::cout << "State after erase(12):" << std::endl;
  avl.print();
  avl.erase(3);
  std::cout << "State after erase(3):" << std::endl;
  avl.print();
  avl.erase(6);
  std::cout << "State after erase(6):" << std::endl;
  avl.print();
  avl.erase(4);
  std::cout << "State after erase(4):" << std::endl;
  avl.print();
  avl.erase(8);
  std::cout << "State after erase(8):" << std::endl;
  avl.print();
  avl.erase(11);
  std::cout << "State after erase(11):" << std::endl;
  avl.print();
  avl.erase(13);
  std::cout << "State after erase(13):" << std::endl;
  avl.print();

  return 0;
}