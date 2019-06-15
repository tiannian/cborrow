#ifndef CBORROW_BORROW_H
#define CBORROW_BORROW_H

#include <config.h>

namespace CBORROW_NAMESPACE_NAME {

template <class _T>
class binder;

template <class _T>
class borrow;

namespace _internal {

struct _linked_header {
    // left pointer
    _linked_header *lptr;

    // right pointer
    _linked_header *rptr;
    
    // first constructer
    _linked_header(): lptr(this), rptr(this) {}
    
    // add new node
    _linked_header(const _linked_header &h): 
        lptr(const_cast<_linked_header*>(&h)),
        rptr(h.rptr) { 
        _insert_right(const_cast<_linked_header &>(h));
    }

    // delete currect node
    ~_linked_header() {
        auto l = this->lptr;
        auto r = this->rptr;
        l->rptr = r;
        r->lptr = l;
    }

    // insert right
    void _insert_right(_linked_header &h) {
        auto next = h.rptr;
        h.rptr = this;
        next->lptr = this;
    }

    // map to iterate
    void map(function<void(_linked_header*)> h) {
        auto p = this;
        do {
            h(p);
            p = p->lptr;
        } while (p->lptr != this);
        h(p);
    }
};

struct _binder_base: _linked_header {
    // reference count
    size_t refcount;

    // inital binder
    _binder_base(): _linked_header(), refcount(0) {}

    // copying is not allowed
    _binder_base(const _binder_base &_b) = delete;

    // release binder
    ~_binder_base() = default;
};

struct _borrow_base: _linked_header {
    // binder pointer
    _binder_base *binder_ptr;
    
    // flag of safety
    bool safe;

    // must use binder to inital
    _borrow_base() = delete;

    // inital by binder
    _borrow_base(const _binder_base &b): 
        _linked_header(b),
        binder_ptr(const_cast<_binder_base*>(&b)),
        safe(true) {
        ++ binder_ptr->refcount;
    }

    _borrow_base(const _borrow_base &b) = delete;

    ~_borrow_base() {
        -- binder_ptr->refcount;
    }
};

template <class T>
struct _basic_binder: _binder_base {
    using _T = typename remove_reference<T>::type;
    // store value
    uint8_t mem[sizeof(_T)];
    _T *value;

    template <class ... Args>
    void make(Args&& ... args) {
        new(mem)_T(forward<Args>(args) ...);
    }

    // create mem block
    _basic_binder(): value(static_cast<_T*>(mem)) {}

    // can't copy
    _basic_binder(const _basic_binder &) = delete;

    // delete all
    ~_basic_binder() {
        // set false
        _linked_header *p = static_cast<_linked_header*>(this);
        while(p->lptr != this) {
            p = p ->lptr;
            _borrow_base *pb = static_cast<_borrow_base*>(p);
            pb->safe = false;
        }
    }
};

template <>
struct _basic_binder<void>: _binder_base {
    // create mem block
    _basic_binder() = default;

    // can't copy
    _basic_binder(const _basic_binder &) = delete;

    // delete all
    ~_basic_binder() {
        // set false
        _linked_header *p = static_cast<_linked_header*>(this);
        while(p->lptr != this) {
            p = p ->lptr;
            _borrow_base *pb = static_cast<_borrow_base*>(p);
            pb->safe = false;
        }
    }
};

template <class T>
struct _basic_borrow: _borrow_base {
    _basic_borrow() = delete;
    _basic_borrow(const _basic_binder<T> &b): _borrow_base(b) {}
    _basic_borrow(const _basic_borrow &) = delete;
    ~_basic_borrow() = default;
};

}

template <class T>
class binder: _internal::_basic_binder<T> {
};

}

#endif

