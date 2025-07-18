# AVL tree (self-balancing binary search tree)

Implementation of AVL tree without recursions and unpredictable iterator invalidation.

## Supported functions

### Constructors
```
(1)  AVL_Tree();
(2)  AVL_Tree(AVL_Tree &&other);
```
1\) Constructs empty tree.  
2\) Move constructor. Constructs tree with the contents of `other` using move semantics.  

### Destructor
```
~AVL_Tree();
```

### Operator=
```
(1)  AVL_Tree& operator= (AVL_Tree &&rhs);
```
1\) Move assignment operator. Replaces the contents of `*this` with the contents of `other` using move semantics.  

### Iterators
```
(1)  const_iterator root() const &;
(2)  iterator root() &;
(3)  const_iterator end() const &;
(4)  iterator end() &;
```
1,2\) Returns iterator to the root element of `*this`.  
3,4\) Returns iterator past the last element of `*this`.  

### Modifiers
```
(1)  iterator insert(KeyT key) &;
(2)  bool erase(KeyT key) &;
```
1\) Attempts to insert element into `*this`.  
    If `*this` already contains an element with an equivalent key, does nothing.  
    Otherwise, inserts the element into `*this` and performs rebalancing according to the AVL balance factor.  
    No iterators are invalidated.  
    Returns iterator to the newly created element or to the already existing element with an equivalent key if no insertion was performed.  

2\) Attempts to remove the element with an equivalent key from `*this`.  
    If `*this` doesn't contain an element with an equivalent key, does nothing.  
    Otherwise, removes the element from `*this` and performs rebalancing according to the AVL balance factor.  
    Iterator to the erased element is invalidated. Other iterators are not affected.  
    Returns `true` if the element was removed, otherwise, returns `false`.  

### Lookup
```
(1)  bool empty() const;
(2)  bool contains(KeyT key) const;
(3)  const_iterator find(KeyT key) const &;
(4)  iterator find(KeyT key) &;
(5)  const_iterator lower_bound(KeyT key) const &;
(6)  iterator lower_bound(KeyT key) &;
(7)  const_iterator lower_bound(KeyT key, const_iterator root) const &;
(8)  iterator lower_bound(KeyT key, iterator root) &;
(9) const_iterator upper_bound(KeyT key) const &;
(10) iterator upper_bound(KeyT key) &;
(11)  const_iterator upper_bound(KeyT key, const_iterator root) const &;
(12) iterator upper_bound(KeyT key, iterator root) &;
```
1\) Checks if `*this` has no elements.  
2\) Checks if `*this` contains an element with key equivalent to `key`.  
3,4\) Finds an element with key equivalent to `key`.  
5,6\) Finds the smallest element in the tree that is not less than `key`.  
7,8\) Finds the smallest element in subtree with the root equivalent to `root` that is not less than `key`.  
9,10\) Finds the smallest element in the tree that is greater than `key`.  
11,12\) Finds the smallest element in subtree with the root equivalent to `root` that is greater than `key`.  
