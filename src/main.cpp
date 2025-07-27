#include <iostream>
#include "tree.hpp"

using SearchTrees::BST_Tree;
using SearchTrees::AVL_Tree;

int main() {
  BST_Tree<int> bst{};
  bst.insert(11);
  bst.insert(8);
  bst.insert(9);
  bst.insert(4);
  bst.insert(3);
  bst.insert(6);
  bst.insert(5);
  bst.insert(13);
  bst.insert(16);
  bst.insert(14);
  bst.insert(10);

  std::cout << "BST tree after insertions:" << std::endl << bst << std::endl;

  BST_Tree<int> bst_copy{bst};

  bst.erase(12);
  bst.erase(3);
  bst.erase(6);
  bst.erase(4);
  bst.erase(8);
  bst.erase(11);
  bst.erase(13);

  std::cout << "BST tree after erases:" << std::endl << bst << std::endl;

  std::cout << "Copy of BST tree before erases:" << std::endl << bst_copy << std::endl;


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

  std::cout << "AVL tree after insertions:" << std::endl << avl << std::endl;

  AVL_Tree<int> avl_copy{avl};

  avl.erase(12);
  avl.erase(3);
  avl.erase(6);
  avl.erase(4);
  avl.erase(8);
  avl.erase(11);
  avl.erase(13);

  std::cout << "AVL tree after erases:" << std::endl << avl << std::endl;

  std::cout << "Copy of AVL tree before erases:" << std::endl << avl_copy << std::endl;

  return 0;
}