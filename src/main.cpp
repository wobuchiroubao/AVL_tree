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

  std::cout << "Tree after insertions:" << std::endl;
  avl.print();

  AVL_Tree<int> avl_copy{avl};

  avl.erase(12);
  avl.erase(3);
  avl.erase(6);
  avl.erase(4);
  avl.erase(8);
  avl.erase(11);
  avl.erase(13);

  std::cout << "Tree after erases:" << std::endl;
  avl.print();

  std::cout << "Copy of tree before erases:" << std::endl;
  avl_copy.print();

  return 0;
}