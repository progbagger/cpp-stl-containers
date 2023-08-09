#ifndef CPP_STL_CONTAINERS_LIST_H_
#define CPP_STL_CONTAINERS_LIST_H_

#include <algorithm>
#include <initializer_list>
#include <iterator>
#include <limits>
#include <utility>

namespace cont {

template <class T>
class list;

template <class T>
struct node {
  node(T* data = nullptr, node* prev = nullptr, node* next = nullptr)
      : data(data), prev(prev), next(next) {}

  ~node() { delete data; }

  T* data;
  node* prev;
  node* next;
};

template <class T>
class list_const_iterator;

template <class T>
class list_iterator {
  friend class list<T>;

 public:
  using difference_type = std::ptrdiff_t;
  using iterator_category = std::bidirectional_iterator_tag;
  using value_type = T;
  using reference = value_type&;
  using pointer = value_type*;
  using node_type = node<value_type>;

  explicit list_iterator(node_type* ptr = nullptr) : ptr_(ptr) {}

  list_iterator& operator++() {
    ptr_ = ptr_->next;
    return *this;
  }

  list_iterator operator++(int) {
    list_iterator tmp(*this);
    ++(*this);
    return tmp;
  }

  list_iterator& operator--() {
    ptr_ = ptr_->prev;
    return *this;
  }

  list_iterator operator--(int) {
    list_iterator tmp(*this);
    --(*this);
    return tmp;
  }

  bool operator==(const list_iterator& other) const noexcept {
    return ptr_ == other.ptr_;
  }

  bool operator!=(const list_iterator& other) const noexcept {
    return !(*this == other);
  }

  reference operator*() const noexcept { return *ptr_->data; }
  pointer operator->() const noexcept { return ptr_->data; }

  operator list_const_iterator<value_type>() {
    return list_const_iterator<value_type>(ptr_);
  }

 protected:
  node_type* ptr_;
};

template <class T>
class list_const_iterator : public list_iterator<T> {
 public:
  using base = list_iterator<T>;
  using value_type = typename base::value_type;
  using node_type = typename base::node_type;
  using reference = const value_type&;
  using pointer = const value_type*;

  explicit list_const_iterator(node_type* ptr = nullptr) : base(ptr) {}

  reference operator*() const noexcept { return *this->ptr_->data; }
  pointer operator->() const noexcept { return this->ptr_->data; }
};

template <class T>
class list {
 public:
  using value_type = T;
  using reference = value_type&;
  using const_reference = const value_type&;
  using pointer = value_type*;
  using const_pointer = const value_type*;
  using size_type = std::size_t;
  using difference_type = std::ptrdiff_t;
  using node_type = node<value_type>;
  using iterator = list_iterator<value_type>;
  using const_iterator = list_const_iterator<value_type>;
  using reverse_iterator = std::reverse_iterator<iterator>;
  using const_reverse_iterator = std::reverse_iterator<const_iterator>;

  list() : size_(), front_(new node_type()), back_(front_) {
    front_->next = front_->prev = front_;
  }

  list(const list& other) : list() { *this = other; }
  list(list&& other) : list() { *this = std::move(other); }

  template <class InputIt>
  list(InputIt first, InputIt second) : list() {
    insert(begin(), first, second);
  }

  list(const std::initializer_list<value_type>& items) : list() {
    insert(begin(), items);
  }

  list& operator=(const list& other) {
    if (this != &other) *this = list(other.begin(), other.end());
    return *this;
  }

  list& operator=(list&& other) noexcept {
    swap(other);
    return *this;
  }

  ~list() {
    clear();
    delete back_;
  }

  void assign(size_type count, const_reference value) {
    clear();
    while (count--) push_back(value);
  }

  template <class InputIt>
  void assign(InputIt first, InputIt last) {
    clear();
    while (first != last) {
      push_back(*first);
      ++first;
    }
  }

  void assign(const std::initializer_list<value_type>& items) {
    assign(items.begin(), items.end());
  }

  reference front() noexcept { return *front_->data; }
  const_reference front() const noexcept { return *front_->data; }

  reference back() noexcept { return *back_->prev->data; }
  const_reference back() const noexcept { return *back_->prev->data; }

  iterator begin() noexcept { return iterator(front_); }
  const_iterator begin() const noexcept { return cbegin(); }
  const_iterator cbegin() const noexcept { return const_iterator(front_); }

  iterator end() noexcept { return iterator(back_); }
  const_iterator end() const noexcept { return cend(); }
  const_iterator cend() const noexcept { return const_iterator(back_); }

  reverse_iterator rbegin() noexcept { return reverse_iterator(end()); }
  const_reverse_iterator rbegin() const noexcept { return crbegin(); }
  const_reverse_iterator crbegin() const noexcept {
    return const_reverse_iterator(cend());
  }

  reverse_iterator rend() noexcept { return reverse_iterator(begin()); }
  const_reverse_iterator rend() const noexcept { return crend(); }
  const_reverse_iterator crend() const noexcept {
    return const_reverse_iterator(cbegin());
  }

  [[nodiscard]] bool empty() const noexcept { return front_ == back_; }
  size_type size() const noexcept { return size_; }
  size_type max_size() const noexcept {
    return std::numeric_limits<difference_type>::max() / sizeof(node_type);
  }

  void clear() {
    while (front_ != back_) {
      node_type* save = front_;
      front_ = front_->next;
      delete save;
    }
    back_->prev = back_->next = back_;
    size_ = size_type();
  }

  iterator insert(const_iterator pos, const_reference value) {
    node_type* node = new node_type(new value_type(value));
    insert_node(pos.ptr_, node);
    return iterator(node);
  }

  iterator insert(const_iterator pos, value_type&& value) {
    node_type* node = new node_type(std::move(new value_type(value)));
    insert_node(pos.ptr_, node);
    return iterator(node);
  }

  template <class InputIt>
  iterator insert(const_iterator pos, InputIt first, InputIt last) {
    if (first == last) return pos;

    iterator result = insert(pos, *first);
    ++first;

    while (first != last) {
      insert(pos, *first);
      ++first;
    }
    return result;
  }

  iterator insert(const_iterator pos,
                  const std::initializer_list<value_type>& items) {
    return insert(pos, items.begin(), items.end());
  }

  template <class... Args>
  iterator emplace(const_iterator pos, Args&&... args) {
    node_type* node = new node_type(std::forward<Args>(args)...);
    insert_node(pos.ptr_, node);
    return iterator(node);
  }

  iterator erase(const_iterator pos) {
    if (front_ == back_) return end();

    iterator result(extract_node(pos.ptr_));
    delete pos.ptr_;
    return result;
  }

  iterator erase(const_iterator first, const_iterator last) {
    if (first == last) return iterator(last.ptr_);

    iterator result = erase(first);
    ++first;
    while (first != last) {
      result = erase(result);
      ++first;
    }
    return result;
  }

  void push_back(const_reference value) { insert(end(), value); }
  void push_back(value_type&& value) { insert(end(), std::move(value)); }

  template <class... Args>
  reference emplace_back(Args&&... args) {
    node_type* node =
        new node_type(new value_type(std::forward<Args>(args)...));
    insert_node(back_, node);
    return *node->data;
  }

  void pop_back() { erase(iterator(back_->prev)); }

  void push_front(const_reference value) { insert(begin(), value); }
  void push_front(value_type&& value) { insert(begin(), std::move(value)); }

  template <class... Args>
  reference emplace_front(Args&&... args) {
    node_type* node =
        new node_type(new value_type(std::forward<Args>(args)...));
    insert_node(front_, node);
    return *node->data;
  }

  void pop_front() { erase(begin()); }

  void resize(size_type count, const_reference value = value_type()) {
    while (size_ < count) push_back(value);
    while (size_ > count) pop_back();
  }

  void swap(list& other) noexcept {
    if (this != &other) {
      std::swap(size_, other.size_);
      std::swap(front_, other.front_);
      std::swap(back_, other.back_);
    }
  }

  void merge(list& other) { merge(other, std::less<value_type>()); }

  template <class Compare>
  void merge(list& other, Compare comp) {
    iterator i1 = begin(), i2 = other.begin();
    while (i1 != end() && i2 != other.begin()) {
      if (comp(*i2, *i1)) {
        i2 = iterator(extract_node(i2.ptr_));
        insert_node(i1.ptr_, i2.ptr_);
      } else {
        ++i1;
      }
    }
    while (i2 != end()) {
      i2 = iterator(extract_node(i2.ptr_));
      insert_node(back_, i2.ptr_);
    }
  }

  void splice(const_iterator pos, list& other, const_iterator first,
              const_iterator last) {
    if (this != &other) {
      while (first != last) {
        iterator save = first;
        first = iterator(extract_node(first.ptr_));
        insert_node(pos.ptr_, save.ptr_);
      }
    }
  }

  void splice(const_iterator pos, list& other, const_iterator it) {
    splice(pos, other, it, other.end());
  }

  void splice(const_iterator pos, list& other) {
    splice(pos, other, other.begin());
  }

  void reverse() noexcept {
    iterator p = begin();
    while (p != end()) {
      std::swap(p.ptr_->prev, p.ptr_->next);
      --p;
    }
    std::swap(back_->next, back_->prev);
    std::swap(front_, back_->prev);
  }

  void unique() { unique(std::equal_to<value_type>()); }

  template <class BinaryPredicate>
  void unique(BinaryPredicate p) {
    if (size_ <= 1) return;

    iterator i = ++begin();
    while (i != end())
      if (p(*i, *(--iterator(i)))) i = erase(i);
  }

  void sort() { sort(std::less<value_type>()); }

  template <class Compare>
  void sort(Compare comp) {
    if (size_ <= 1) return;

    iterator i1 = begin();
    while (i1 != --end()) {
      iterator i2 = ++iterator(i1);
      iterator min = i1;
      while (i2 != end()) {
        if (comp(*i2, *i1)) min = i2;
        ++i2;
      }
      if (min != i1) swap_nodes_by_iterators(&i1, &min);
    }
  }

  friend bool operator==(const list& l1, const list& l2) {
    return std::equal(l1.begin(), l1.end(), l2.begin(), l2.end());
  }

  friend bool operator!=(const list& l1, const list& l2) { return !(l1 == l2); }

  friend bool operator<(const list& l1, const list& l2) {
    return std::lexicographical_compare(l1.begin(), l1.end(), l2.begin(),
                                        l2.end());
  }

  friend bool operator<=(const list& l1, const list& l2) { return !(l1 > l2); }
  friend bool operator>(const list& l1, const list& l2) { return l2 < l1; }
  friend bool operator>=(const list& l1, const list& l2) { return !(l1 < l2); }

 private:
  void insert_node(node_type* pos, node_type* node) {
    node->prev = pos->prev;
    node->next = pos;
    pos->prev = pos->prev->next = node;

    if (pos == front_) front_ = node;

    ++size_;
  }

  node_type* extract_node(node_type* pos) {
    node_type* result = pos->next;
    if (pos == front_) front_ = front_->next;

    pos->prev->next = pos->next;
    pos->next->prev = pos->prev;

    --size_;
    return result;
  }

  static void swap_nodes_by_iterators(iterator* i1, iterator* i2) {
    std::swap(i1->ptr_->next, i2->ptr_->next);
    std::swap(i1->ptr_->prev, i2->ptr_->prev);

    std::swap(i1->ptr_->prev->next, i2->ptr_->prev->next);
    std::swap(i1->ptr_->next->prev, i2->ptr_->next->prev);

    std::swap(*i1, *i2);
  }

  size_type size_;
  node_type* front_;
  node_type* back_;
};

// deduction guides

template <class InputIt>
list(InputIt, InputIt)
    -> list<typename std::iterator_traits<InputIt>::value_type>;

}  // namespace cont

#endif  // CPP_STL_CONTAINERS_LIST_H_
