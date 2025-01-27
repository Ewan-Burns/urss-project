#include <cstddef> // for std::size_t
#include <stdexcept> // for std::out_of_range
#include <limits> // for std::numeric_limits
#include <utility> // for std::swap

template <typename T>
class basic_vector {
public:
    // types
    using value_type = T;
    using size_type = std::size_t;
    using difference_type = std::ptrdiff_t;
    using reference = T&;
    using const_reference = const T&;
    using pointer = T*;
    using const_pointer = const T*;
    
    // iterator
    class iterator {
    public:
        using iterator_category = std::random_access_iterator_tag;
        using value_type = T;
        using difference_type = std::ptrdiff_t;
        using pointer = T*;
        using reference = T&;
        
        iterator() : ptr(nullptr) {}
        iterator(pointer p) : ptr(p) {}
        iterator(const iterator& other) : ptr(other.ptr) {}
        
        iterator& operator=(const iterator& other) {
            ptr = other.ptr;
            return *this;
        }
        
        iterator& operator++() {
            ++ptr;
            return *this;
        }
        
        iterator operator++(int) {
            iterator tmp(*this);
            ++ptr;
            return tmp;
        }
        
        iterator& operator--() {
            --ptr;
            return *this;
        }
        
        iterator operator--(int) {
            iterator tmp(*this);
            --ptr;
            return tmp;
        }
        
        iterator& operator+=(difference_type n) {
            ptr += n;
            return *this;
        }
        
        iterator operator+(difference_type n) const {
            iterator tmp(*this);
            return tmp += n;
        }
        
        iterator& operator-=(difference_type n) {
            ptr -= n;
            return *this;
        }
        
        iterator operator-(difference_type n) const {
            iterator tmp(*this);
            return tmp -= n;
        }
        
        difference_type operator-(const iterator& other) const {
            return ptr - other.ptr;
        }
        
        reference operator*() const {
            return *ptr;
        }
        
        pointer operator->() const {
            return ptr;
        }
        
        bool operator==(const iterator& other) const {
            return ptr == other.ptr;
        }
        
        bool operator!=(const iterator& other) const {
            return ptr != other.ptr;
        }
        
        bool operator<(const iterator& other) const {
            return ptr < other.ptr;
        }
        
        bool operator>(const iterator& other) const {
            return ptr > other.ptr;
        }
        
        bool operator<=(const iterator& other) const {
            return ptr <= other.ptr;
        }
        
        bool operator>=(const iterator& other) const {
            return ptr >= other.ptr;
        }
        
    private:
        pointer ptr;
    };
    
    // constructors
    basic_vector() : udata(nullptr), sz(0), cap(0) {}
    explicit basic_vector(size_type n) : udata(new T[n]), sz(n), cap(n) {}
    basic_vector(const basic_vector& other) : udata(new T[other.sz]), sz(other.sz), cap(other.sz) {
        for (size_type i = 0; i < sz; ++i) {
            udata[i] = other.udata[i];
        }
    }
    basic_vector(basic_vector&& other) noexcept : udata(other.udata), sz(other.sz), cap(other.cap) {
        other.udata = nullptr;
        other.sz = 0;
        other.cap = 0;
    }
    
    // destructor
    ~basic_vector() {
        delete[] udata;
    }
    
    // assignment operators
    basic_vector& operator=(const basic_vector& other) {
        if (this != &other) {
            delete[] udata;
            udata = new T[other.sz];
            sz = other.sz;
            cap = other.sz;
            for (size_type i = 0; i < sz; ++i) {
                udata[i] = other.udata[i];
            }
        }
        return *this;
    }
    
    basic_vector& operator=(basic_vector&& other) noexcept {
        if (this != &other) {
            delete[] udata;
            udata = other.udata;
            sz = other.sz;
            cap = other.cap;
            other.udata = nullptr;
            other.sz = 0;
            other.cap = 0;
        }
        return *this;
    }
    
    // element access
    reference operator[](size_type n) {
        return udata[n];
    }
    
    reference at(size_type n) {
        if (n >= sz) {
            throw std::out_of_range("basic_vector::at");
        }
        return udata[n];
    }
    
    // iterators
    iterator begin() noexcept {
        return iterator(udata);
    }
    
    
    iterator end() noexcept {
        return iterator(udata + sz);
    }
    
    size_type size() const noexcept {
        return sz;
    }

    void reserve(size_type n) {
        if (n > cap) {
            pointer new_data = new T[n];
            for (size_type i = 0; i < sz; ++i) {
                new_data[i] = udata[i];
            }
            delete[] udata;
            udata = new_data;
            cap = n;
        }
    }

    void push_back(const T& val) {
        if (sz == cap) {
            reserve(cap == 0 ? 1 : cap * 2);
        }
        udata[sz] = val;
        ++sz;
    }
    
    // modifiers
    void clear() noexcept {
        sz = 0;
    }
    
    iterator insert(iterator pos, const T& val) {
        difference_type index = pos - begin();
        if (sz == cap) {
            reserve(cap == 0 ? 1 : cap * 2);
        }
        for (size_type i = sz; i > static_cast<size_type>(index); --i) {
            udata[i] = udata[i-1];
        }
        udata[index] = val;
        ++sz;
        return iterator(udata + index);
    }
    
    iterator erase(iterator pos) {
        difference_type index = pos - begin();
        for (size_type i = static_cast<size_type>(index); i < sz-1; ++i) {
            udata[i] = udata[i+1];
        }
        --sz;
        return iterator(udata + index);
    }
    
    void swap(basic_vector& other) noexcept {
        std::swap(udata, other.udata);
        std::swap(sz, other.sz);
        std::swap(cap, other.cap);
    }

    iterator insert(iterator pos, size_type n, const T& val) {
        difference_type index = pos - begin();
        if (sz + n > cap) {
            reserve(sz + n);
        }
        for (size_type i = sz + n - 1; i > static_cast<size_type>(index + n - 1); --i) {
            udata[i] = udata[i-n];
        }
        for (size_type i = static_cast<size_type>(index); i < static_cast<size_type>(index + n); ++i) {
            udata[i] = val;
        }
        sz += n;
        return iterator(udata + index);
    }

    template <typename InputIterator>
    iterator insert(iterator pos, InputIterator first, InputIterator last) {
        difference_type index = pos - begin();
        size_type n = last - first;
        if (sz + n > cap) {
            reserve(sz + n);
        }
        for (size_type i = sz + n - 1; i > static_cast<size_type>(index + n - 1); --i) {
            udata[i] = udata[i-n];
        }
        for (size_type i = static_cast<size_type>(index); i < static_cast<size_type>(index + n); ++i) {
            udata[i] = *first;
            ++first;
        }
        sz += n;
        return iterator(udata + index);
    }

    iterator erase(iterator first, iterator last) {
        difference_type index = first - begin();
        difference_type n = last - first;
        for (size_type i = static_cast<size_type>(index); i < sz - n; ++i) {
            udata[i] = udata[i+n];
        }
        sz -= n;
        return iterator(udata + index);
    }
    
private:
    pointer udata;
    size_type sz;
    size_type cap;
};

