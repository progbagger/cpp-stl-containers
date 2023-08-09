#ifndef CPP_STR_CONTAINERS_AVL_TREE_H_
#define CPP_STR_CONTAINERS_AVL_TREE_H_

#include <initializer_list>
#include <limits>
#include <memory>
#include <stdexcept>

namespace cont {

template <class T>
struct tree_node {
  explicit tree_node(T* data = nullptr, tree_node* parent = nullptr,
                     tree_node* left = nullptr, tree_node* right = nullptr,
                     int balance_factor = 0)
      : data(data),
        parent(parent),
        left(left),
        right(right),
        balance_factor(balance_factor) {}

  ~tree_node() { delete data; }

  T* data;
  tree_node* parent;
  tree_node* left;
  tree_node* right;
  int balance_factor;  // -2, -1, 0, 1, 2
};

template <class T, class Key, class KeyOfValue, class Compare>
class avl_tree;

template <class T>
class tree_const_iterator;

template <class T>
class tree_iterator {
  template <class BT, class Key, class KeyOfValue,
            class Compare>  // for gaining access to ptr_
  friend class avl_tree;    // in tree
 public:
  using node_type = tree_node<T>;
  using value_type = T;
  using reference = value_type&;
  using pointer = value_type*;
  using difference_type = std::ptrdiff_t;
  using iterator_category = std::bidirectional_iterator_tag;

  explicit tree_iterator(node_type* ptr) : ptr_(ptr) {}

  tree_iterator& operator++() {
    if (ptr_->right) {
      ptr_ = ptr_->right;
      while (ptr_->left) ptr_ = ptr_->left;
    } else {
      while (ptr_->parent && ptr_->parent->right == ptr_) ptr_ = ptr_->parent;
      if (ptr_->parent) ptr_ = ptr_->parent;
    }
    return *this;
  }

  tree_iterator operator++(int) {
    tree_iterator tmp(*this);
    ++(*this);
    return tmp;
  }

  tree_iterator& operator--() {
    if (ptr_->left) {
      ptr_ = ptr_->left;
      while (ptr_->right) ptr_ = ptr_->right;
    } else {
      while (ptr_->parent && ptr_->parent->left == ptr_) ptr_ = ptr_->parent;
      if (ptr_->parent) ptr_ = ptr_->parent;
    }
    return *this;
  }

  tree_iterator operator--(int) {
    tree_iterator tmp(*this);
    --(*this);
    return tmp;
  }

  friend bool operator==(const tree_iterator& lhs,
                         const tree_iterator& rhs) noexcept {
    return lhs.ptr_ == rhs.ptr_;
  }

  friend bool operator!=(const tree_iterator& lhs,
                         const tree_iterator& rhs) noexcept {
    return !(lhs == rhs);
  }

  reference operator*() const noexcept { return *ptr_->data; }
  pointer operator->() const noexcept { return ptr_->data; }

  operator tree_const_iterator<value_type>() const {
    return tree_const_iterator<value_type>(ptr_);
  }

 protected:
  node_type* ptr_;
};

template <class T>
class tree_const_iterator : public tree_iterator<T> {
 public:
  using value_type = typename tree_iterator<T>::value_type;
  using difference_type = typename tree_iterator<T>::difference_type;
  using iterator_category = typename tree_iterator<T>::iterator_category;
  using node_type = typename tree_iterator<T>::node_type;
  using reference = const value_type&;
  using pointer = const value_type*;

  explicit tree_const_iterator(node_type* ptr_ = nullptr)
      : tree_iterator<value_type>(ptr_) {}

  reference operator*() const noexcept { return *this->ptr_->data; }
  pointer operator->() const noexcept { return this->ptr_->data; }
};

template <class Key, class Value, class KeyOfValue, class Compare>
class avl_tree {
 public:
  using key_type = Key;
  using value_type = Value;
  using reference = value_type&;
  using const_reference = const value_type&;
  using size_type = std::size_t;
  using difference_type = std::ptrdiff_t;
  using key_compare = Compare;
  using key_retractor = KeyOfValue;
  using node_type = tree_node<value_type>;
  using iterator = tree_iterator<value_type>;
  using const_iterator = tree_const_iterator<value_type>;
  using reverse_iterator = std::reverse_iterator<iterator>;
  using const_reverse_iterator = std::reverse_iterator<const_iterator>;

  avl_tree()
      : comparator_(),
        key_retractor_(),
        size_(),
        root_(new node_type()),
        first_(root_),
        last_(first_) {
    root_->parent = last_;
    root_->right = first_;
  }

  avl_tree(const avl_tree& other) : avl_tree() { *this = other; }
  avl_tree(avl_tree&& other) noexcept : avl_tree() { *this = std::move(other); }

  avl_tree& operator=(const avl_tree& other) {
    if (this != &other) {
      clear();
      for (const auto& el : other) insert(el);
    }
    return *this;
  }
  avl_tree& operator=(avl_tree&& other) noexcept {
    swap(other);
    return *this;
  }

  ~avl_tree() {
    clear();
    delete last_;
  }

  std::pair<iterator, bool> insert(const value_type& value,
                                   bool duplicates = false) {
    node_type* pos = find_element(key_retractor_(value), duplicates);
    if (!duplicates && pos != last_ &&
        is_keys_equal(key_retractor_(*pos->data), key_retractor_(value)))
      return {iterator(pos), false};

    node_type* new_node = new node_type(new value_type(value));
    std::pair<node_type*, bool> insert_result = insert_node(pos, new_node);
    return {iterator(insert_result.first), insert_result.second};
  }

  std::pair<iterator, bool> insert(value_type&& value,
                                   bool duplicates = false) {
    node_type* pos = find_element(key_retractor_(value), duplicates);
    if (!duplicates && pos != last_ &&
        is_keys_equal(key_retractor_(*pos->data), key_retractor_(value)))
      return {iterator(pos), false};

    node_type* new_node = new node_type(new value_type(std::move(value)));
    std::pair<node_type*, bool> insert_result = insert_node(pos, new_node);
    return {iterator(insert_result.first), insert_result.second};
  }

  value_type& at(const key_type& key) {
    node_type* pos = find_element(key);
    if (pos != last_ && is_keys_equal(key_retractor_(*pos->data), key))
      return *pos->data;
    throw std::out_of_range("There are no value with this key");
  }

  const value_type& at(const key_type& key) const {
    node_type* pos = find_element(key);
    if (pos != last_ && is_keys_equal(key_retractor_(*pos->data), key))
      return *pos->data;
    throw std::out_of_range("There are no value with this key");
  }

  iterator erase(const_iterator pos) { return delete_node(pos.ptr_); }

  size_type erase(const key_type& key) {
    size_type result = 0;
    iterator pos = find(key);
    while (pos != end() && is_keys_equal(key_retractor_(*pos), key)) {
      pos = delete_node(pos.ptr_);
      ++result;
    }
    return result;
  }

  iterator find(const key_type& key) {
    node_type* node = find_element(key);
    if (node->data && is_keys_equal(key_retractor_(*node->data), key))
      return iterator(node);
    return iterator(last_);
  }

  const_iterator find(const key_type& key) const {
    node_type* node = find_element(key);
    if (node->data && is_keys_equal(key_retractor_(*node->data), key))
      return const_iterator(node);
    return const_iterator(last_);
  }

  bool contains(const key_type& key) const { return find(key) != end(); }

  iterator lower_bound(const key_type& key) { return search_bounds(key, true); }

  const_iterator lower_bound(const key_type& key) const {
    return search_bounds(key, true);
  }

  iterator upper_bound(const key_type& key) {
    return search_bounds(key, false);
  }

  const_iterator upper_bound(const key_type& key) const {
    return search_bounds(key, false);
  }

  std::pair<iterator, iterator> equal_range(const key_type& key) {
    return {lower_bound(key), upper_bound(key)};
  }

  std::pair<const_iterator, const_iterator> equal_range(
      const key_type& key) const {
    return {lower_bound(key), upper_bound(key)};
  }

  const_iterator cbegin() const noexcept { return const_iterator(first_); }
  const_iterator begin() const noexcept { return cbegin(); }

  const_reverse_iterator crbegin() const noexcept {
    return const_reverse_iterator(cend());
  }
  const_reverse_iterator rbegin() const noexcept { return crbegin(); }

  const_iterator cend() const noexcept { return const_iterator(last_); }
  const_iterator end() const noexcept { return cend(); }

  const_reverse_iterator crend() const noexcept {
    return const_reverse_iterator(cbegin());
  }
  const_reverse_iterator rend() const noexcept { return crend(); }

  iterator begin() noexcept { return iterator(first_); }
  iterator end() noexcept { return iterator(last_); }

  reverse_iterator rbegin() noexcept { return reverse_iterator(end()); }
  reverse_iterator rend() noexcept { return reverse_iterator(begin()); }

  size_type size() const noexcept { return size_; }
  size_type max_size() const noexcept {
    return std::numeric_limits<difference_type>::max() / sizeof(node_type);
  }
  bool empty() const noexcept { return !size_; }

  void clear() noexcept {
    clear_recursively(root_);
    root_ = first_ = last_;
    last_->right = first_;
    size_ = size_type();
  }

  void swap(avl_tree& other) noexcept {
    if (this != &other) {
      std::swap(size_, other.size_);
      std::swap(root_, other.root_);
      std::swap(last_, other.last_);
      std::swap(first_, other.first_);
    }
  }

  void merge(avl_tree& other) {
    if (this != &other) {
      if (empty()) {
        // small optimization
        swap(other);
        return;
      }

      auto i = other.begin();
      while (i != other.end()) {
        node_type* node = find_element(key_retractor_(*i));
        if (node != last_) {
          if (!is_keys_equal(key_retractor_(*node->data), key_retractor_(*i))) {
            node_type* to_insert = i.ptr_;
            ++i;  // need to change iterator's pos before deletion
            other.extract_node(to_insert);
            to_insert->right = to_insert->left = to_insert->parent = nullptr;
            to_insert->balance_factor = 0;
            insert_node(node, to_insert);
          } else {
            ++i;
          }
        }
      }
    }
  }

  friend bool operator==(const avl_tree& tr1, const avl_tree& tr2) {
    return std::equal(tr1.begin(), tr1.end(), tr2.begin(), tr2.end());
  }

  friend bool operator!=(const avl_tree& tr1, const avl_tree& tr2) {
    return !(tr1 == tr2);
  }

  friend bool operator<(const avl_tree& tr1, const avl_tree& tr2) {
    return std::lexicographical_compare(tr1.begin(), tr1.end(), tr2.begin(),
                                        tr2.end());
  }

  friend bool operator>(const avl_tree& tr1, const avl_tree& tr2) {
    return tr2 < tr1;
  }

  friend bool operator<=(const avl_tree& tr1, const avl_tree& tr2) {
    return !(tr1 > tr2);
  }

  friend bool operator>=(const avl_tree& tr1, const avl_tree& tr2) {
    return !(tr1 < tr2);
  }

 private:
  enum rotation_side : bool { kLeft, kRight };

  void clear_recursively(node_type* node) noexcept {
    if (!node || node == last_) return;

    clear_recursively(node->left);
    clear_recursively(node->right);

    delete node;
  }

  iterator search_bounds(const key_type& key, bool lower) const {
    node_type* appropriate_leaf = find_element(key);
    if (appropriate_leaf == last_) return iterator(last_);

    node_type* prev = appropriate_leaf;
    while (appropriate_leaf && appropriate_leaf != last_ &&
           (comparator_(key_retractor_(*appropriate_leaf->data), key) ||
            is_keys_equal(key_retractor_(*appropriate_leaf->data), key))) {
      prev = appropriate_leaf;
      appropriate_leaf = appropriate_leaf->right;
    }

    if (comparator_(key_retractor_(*prev->data), key)) return iterator(last_);
    if (lower) return iterator(prev);
    if (!appropriate_leaf) return iterator(last_);
    return iterator(appropriate_leaf);
  }

  node_type* find_element(const Key& key, bool duplicates = false) const {
    node_type* current = root_;
    while (
        current != last_ && (current->left || current->right) &&
        (duplicates || !is_keys_equal(key_retractor_(*current->data), key))) {
      // saving comparison result to make one comparison at each iteration
      bool compare_result = comparator_(key, key_retractor_(*current->data));
      if (current->left && current->left != last_ && compare_result) {
        current = current->left;
      } else if (current->right && current->right != last_ && !compare_result) {
        current = current->right;
      } else {
        break;
      }
    }
    return current;
  }

  std::pair<node_type*, bool> insert_node(node_type* pos, node_type* node) {
    if (pos == last_) {
      // handle empty tree
      node->parent = nullptr;
      node->right = last_;
      last_->parent = first_ = root_ = node;
    } else {
      // not empty node
      node->parent = pos;
      if (comparator_(key_retractor_(*node->data),
                      key_retractor_(*pos->data))) {
        // left side
        pos->left = node;
        if (pos == first_) first_ = node;
      } else {
        // right side
        // moving last's node parent
        if (pos == last_->parent) last_->parent = node;
        node->right = pos->right;
        pos->right = node;
      }
    }
    update_bf(node, true);

    first_->left = last_;
    last_->right = first_;
    ++size_;
    return {node, true};
  }

  void recover_last_node() {
    last_->left = nullptr;
    last_->balance_factor = 0;
    last_->right = first_;
  }

  std::pair<node_type*, int> remove_leaf(node_type* pos) {
    std::pair<node_type*, int> result(pos->parent, 0);
    if (pos->parent->left == pos) {
      pos->parent->left = nullptr;
      result.second = 1;
    } else {
      pos->parent->right = nullptr;
      result.second = -1;
    }
    return result;
  }

  void move_last_node(node_type* pos) {
    if (pos->left == last_) {
      node_type* successor = pos->right;
      while (successor->left) successor = successor->left;
      successor->left = last_;
      pos->left = nullptr;
      first_ = successor;
    } else {
      node_type* successor = pos->left;
      while (successor->right) successor = successor->right;
      successor->right = last_;
      last_->parent = successor;
      pos->right = nullptr;
    }
  }

  std::pair<node_type*, int> remove_one_childed_node(node_type* pos) {
    node_type* successor = pos->left ? pos->left : pos->right;
    // moving first node pointer to appropriate location
    if (pos == first_ && pos->parent->left == pos) first_ = pos->parent;
    std::pair<node_type*, int> result(pos->parent, 0);
    if (!(successor == last_ && pos->left == successor))
      // if pos is left and its successor is last_ - do not change parent
      successor->parent = pos->parent;

    // splicing "chain" node
    if (pos != root_ && pos->parent->left == pos) {
      pos->parent->left = successor;
      result.second = 1;
    } else if (pos != root_) {
      pos->parent->right = successor;
      result.second = -1;
    }

    // repoint root pointer if needed
    if (pos == root_) root_ = successor;
    return result;
  }

  std::pair<node_type*, int> remove_two_childed_node(node_type* pos) {
    if (pos->left == pos->right) {
      // deleting 1-sized root
      first_ = root_ = last_;
      return {nullptr, 0};
    }

    if (pos->left == last_ || pos->right == last_) {
      // moving last node pointer and handling one childed deletion
      move_last_node(pos);
      return remove_one_childed_node(pos);
    }

    // true two child deletion
    node_type* successor = pos->right;
    while (successor->left) successor = successor->left;
    successor->balance_factor = pos->balance_factor;
    // bf change always will be -1 in this case because node is always moved
    // from right side
    std::pair<node_type*, int> result(
        successor->parent, successor->parent->left == successor ? 1 : -1);
    if (successor->parent == pos) result.first = successor;
    if (pos->parent) {
      pos->parent->left == pos ? pos->parent->left = successor
                               : pos->parent->right = successor;
    } else {
      root_ = successor;
    }
    if (pos->right != successor) {
      // indirect child - move successor's right to his parent->left
      successor->parent->left = successor->right;
      if (successor->right) successor->right->parent = successor->parent;
      successor->right = pos->right;
      pos->right->parent = successor;
    }
    // move pos' left to successor's left in any case
    pos->left->parent = successor;
    successor->left = pos->left;
    successor->parent = pos->parent;
    return result;
  }

  void extract_node(node_type* pos) {
    if (pos == last_ || !size_) return;

    // for rebalancing
    std::pair<node_type*, int> for_balance(nullptr, 0);
    if (!pos->right && !pos->left)
      for_balance = remove_leaf(pos);
    else if (!pos->left != !pos->right)
      for_balance = remove_one_childed_node(pos);
    else
      for_balance = remove_two_childed_node(pos);

    if (first_ != last_) first_->left = last_;
    recover_last_node();
    --size_;

    update_bf_after_delete(for_balance);
  }

  iterator delete_node(node_type* pos) {
    if (pos == last_ || !size_) return end();
    iterator result = ++iterator(pos);

    extract_node(pos);
    delete pos;

    return result;
  }

  void update_bf(node_type* pos, bool insertion) {
    if (!pos || pos == last_) return;

    while (pos->parent) {
      // changing balance factor
      if (pos->parent->left == pos && insertion)
        --pos->parent->balance_factor;
      else
        ++pos->parent->balance_factor;
      pos = repair(pos->parent);  // if disbalanced - balance
      if ((insertion && pos->balance_factor == 0) ||
          (!insertion && std::abs(pos->balance_factor) <= 1))
        break;
    }
  }

  void update_bf_after_delete(std::pair<node_type*, int> for_balance) {
    node_type* pos = for_balance.first;
    int initial_difference = for_balance.second;

    if (!pos || pos == last_) return;

    // making changes to removed or changed child's parent
    pos->balance_factor += initial_difference;
    pos = repair(pos);

    if (std::abs(pos->balance_factor) <= 1) return;

    // balance upper subtrees
    update_bf(pos, false);
  }

  node_type* repair(node_type* pos) {
    // moving to parent
    node_type** target_parent;
    if (pos == root_) {
      target_parent = &root_;
    } else {
      target_parent =
          pos->parent->left == pos ? &pos->parent->left : &pos->parent->right;
    }

    // repairing if needed
    if (pos->balance_factor > 1) {
      if (pos->right->balance_factor >= 0)
        *target_parent = rotate_once(pos, kLeft);
      else
        *target_parent = rotate_twice(pos, kRight);
    } else if (pos->balance_factor < -1) {
      if (pos->left->balance_factor <= 0)
        *target_parent = rotate_once(pos, kRight);
      else
        *target_parent = rotate_twice(pos, kLeft);
    } else {
      return pos;
    }

    return *target_parent;
  }

  node_type* rotate_once(node_type* node, rotation_side rotation_side,
                         bool change_bf = true) {
    // left or right side of X - node that needs to be rotated
    node_type** x_side = rotation_side == kLeft ? &node->right : &node->left;
    // middle subtree
    node_type* t23 =
        rotation_side == kLeft ? (*x_side)->left : (*x_side)->right;
    // saving side's pointer to further operations
    node_type* z = *x_side;

    *x_side = t23;
    if (t23) t23->parent = node;
    rotation_side == kLeft ? z->left = node : z->right = node;
    z->parent = node->parent;
    node->parent = z;
    if (change_bf && z->balance_factor == 0) {
      node->balance_factor = rotation_side == kLeft ? 1 : -1;
      z->balance_factor = rotation_side == kLeft ? -1 : 1;
    } else if (change_bf) {
      node->balance_factor = z->balance_factor = 0;
    }
    return z;
  }

  node_type* rotate_twice(node_type* node, rotation_side rotation_side) {
    // saving pointers to rearrange bf's
    node_type* x = node;
    node_type* z = rotation_side == kRight ? node->right : node->left;
    node_type* y = rotation_side == kRight ? z->left : z->right;

    // performing rotations
    if (rotation_side == kRight) {
      node->right = rotate_once(node->right, kRight, false);
      rotate_once(node, kLeft, false);
    } else {
      node->left = rotate_once(node->left, kLeft, false);
      rotate_once(node, kRight, false);
    }

    // adjusting weights
    if (y->balance_factor == 0) {
      x->balance_factor = z->balance_factor = 0;
    } else if (y->balance_factor > 0) {
      x->balance_factor = -1;
      z->balance_factor = 0;
    } else {
      x->balance_factor = 0;
      z->balance_factor = +1;
    }
    y->balance_factor = 0;

    return y;
  }

  // to use only one provided comparison for keys
  bool is_keys_equal(const key_type& key1, const key_type& key2) const {
    return !comparator_(key1, key2) && !comparator_(key2, key1);
  }

  key_compare comparator_;
  key_retractor key_retractor_;
  size_type size_;
  node_type* root_;
  node_type* first_;
  node_type* last_;
};

}  // namespace cont

#endif  // CPP_STR_CONTAINERS_AVL_TREE_H_
