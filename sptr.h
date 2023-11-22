#ifndef B_1_UNIQUE_PTR_IVAN_H
#define B_1_UNIQUE_PTR_IVAN_H

#include <algorithm>

template <typename T>
class UniquePtr {
 private:
  T* ptr_;

 public:
  UniquePtr() : ptr_(nullptr) {
  }

  explicit UniquePtr(T* p) : ptr_(p) {
  }

  UniquePtr(const UniquePtr<T>&) = delete;

  UniquePtr& operator=(const UniquePtr<T>&) = delete;

  UniquePtr(UniquePtr&& other) noexcept : ptr_(other.ptr_) {
    other.ptr_ = nullptr;
  }

  UniquePtr& operator=(UniquePtr&& other) noexcept {
    if (this != &other) {
      delete ptr_;
      ptr_ = other.ptr_;
      other.ptr_ = nullptr;
    }
    return *this;
  }

  T& operator*() const {
    return *ptr_;
  }

  T* operator->() const {
    return ptr_;
  }

  ~UniquePtr() {
    delete ptr_;
  }
};

struct RefCntBlock {
  size_t strong, weak;
};

template <class T>
class WeakPtr;

template <class T>
class SharedPtr {
  friend class WeakPtr<T>;
 private:
  T* ptr;
  RefCntBlock* block;

 public:
  SharedPtr() noexcept : ptr(), block() {}

  SharedPtr(const SharedPtr& other) noexcept {
    ptr = other.ptr;
    block = other.block;
    if (block) {
      ++block->strong;
    }
  }

  SharedPtr& operator=(const SharedPtr& other) noexcept {
    if (block != other.block) {
      Clean();
    }
    ptr = other.ptr;
    block = other.block;
    if (block) {
      ++block->strong;
    }
    return *this;
  }

  SharedPtr(SharedPtr&& other) noexcept {
    ptr = other.ptr;
    block = other.block;
    if (block) {
      ++block->strong;
    }
    other.Reset();
  };

  SharedPtr& operator=(SharedPtr&& other) noexcept {
    if (block != other.block) {
      Clean();
    }
    ptr = other.ptr;
    block = other.block;
    if (block) {
      ++block->strong;
    }
    other.Reset();
    return *this;
  }

  SharedPtr(T* p) noexcept : ptr(p), block(new RefCntBlock()) {
    ++block->strong;
  }

  SharedPtr(const WeakPtr<T>& other) noexcept;

  T* operator->() { return ptr; }

  T& operator*() { return *ptr; }

  void Reset() noexcept {
    Clean();
    ptr = nullptr;
    block = nullptr;
  }

  void Clean() {
    if (block) {
      --block->strong;
      if (!block->strong) {
        delete ptr;
        if (!block->weak) {
          delete block;
        }
      }
    }
  }
  ~SharedPtr() {
    Clean();
  }
};

template <class T>
class WeakPtr {
  friend class SharedPtr<T>;
 private:
  T* ptr;
  RefCntBlock* block;

 public:
  WeakPtr() noexcept : block() {}

  WeakPtr(const WeakPtr& other) noexcept {
    ptr = other.ptr;
    block = other.block;
    if (block) {
      ++block->weak;
    }
  }

  WeakPtr& operator=(const WeakPtr& other) noexcept {
    if (block != other.block) {
      Clean();
    }
    ptr = other.ptr;
    block = other.block;
    if (block) {
      ++block->weak;
    }
    return *this;
  }

  WeakPtr(WeakPtr&& other) noexcept {
    ptr = other.ptr;
    block = other.block;
    if (block) {
      ++block->weak;
    }
    other.Reset();
  };

  WeakPtr& operator=(WeakPtr&& other) noexcept {
    if (block != other.block) {
      Clean();
    }
    block = other.block;
    ptr = other.ptr;
    if (block) {
      ++block->weak;
    }
    other.Reset();
    return *this;
  }

  WeakPtr(const SharedPtr<T>& other) noexcept {
    ptr = other.ptr;
    block = other.block;
    if (block) {
      ++block->weak;
    }
  };

  WeakPtr& operator=(const SharedPtr<T>& other) noexcept {
    if (block != other.block) {
      Clean();
    }
    block = other.block;
    ptr = other.ptr;
    if (block) {
      ++block->weak;
    }
    return *this;
  }

  void Reset() noexcept {
    Clean();
    block = nullptr;
    ptr = nullptr;
  }

  bool Expired() const noexcept {
    return (!block || !block->strong);
  }

  SharedPtr<T> Lock() noexcept {
    return SharedPtr<T>(*this);
  }

  void Clean() {
    if (block) {
      --block->weak;
      if (!block->strong && !block->weak) {
        delete block;
      }
    }
  }
  ~WeakPtr() noexcept {
    Clean();
  }
};

template <class T>
SharedPtr<T>::SharedPtr(const WeakPtr<T>& other) noexcept {
  ptr = other.ptr;
  block = other.block;
  if (block) {
    ++block->strong;
  }
}

#endif
