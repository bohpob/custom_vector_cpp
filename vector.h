#include <cstdlib>
#include <algorithm>
#include <utility>
#include <memory>
#include <stdexcept>
#include <exception>

namespace test {
    template<typename T, size_t N>
    class vector {
        // Fixed-size buffer used for small amounts of data.
        alignas(T) std::byte storage_[sizeof(T) * N];
        // Pointer to the current data.
        T *data_;
        // Size and capacity of the vector.
        size_t size_, capacity_;

        // Swap two vectors using static buffers.
        void swap_between_static_buffers(vector &);
        // Swap dynamic and static buffers.
        void swap_dynamic_with_static(vector &, vector &);
        // Check if the vector is using the static buffer.
        bool is_using_static_buffer();

    public:
        vector() noexcept; // Default constructor.

        vector(const vector &); // Copy constructor.

        vector &operator=(const vector &); // Copy assignment operator.

        vector(vector &&); // Move constructor.

        vector &operator=(vector &&); // Move assignment operator.

        ~vector(); // Destructor.

        T *data(); //  Pointer to the first element.

        const T *data() const; //  Pointer to the first element (const version).

        T &operator[](size_t);  // Access element by index.

        const T &operator[](size_t) const; // Access element by index (const version).

        void push_back(const T &); // Add an element to the end (copy).

        void push_back(T &&); // Add an element to the end (move).

        template<typename... Ts>
        void emplace_back(Ts &&...); // Inserts an element using emplace semantics.

        void pop_back(); // Remove the last element.

        void clear(); // Clear all elements.

        void reserve(size_t); // Reserve memory for a given capacity.

        [[nodiscard]] size_t capacity() const; // Get the current capacity.

        [[nodiscard]] size_t size() const; // Get the current size.

        void swap(vector &); // Swap two vectors.
    };

    template<typename T, size_t N>
    vector<T, N>::vector() noexcept : storage_{}, data_(reinterpret_cast<T *>(storage_)), size_{0}, capacity_{N} {
    }

    template<typename T, size_t N>
    vector<T, N>::vector(const vector &other) : storage_{}, data_(reinterpret_cast<T *>(storage_)), size_{0},
                                                capacity_{N} {
        if (other.size() > N) {
            data_ = static_cast<T *>(::operator new(other.capacity() * sizeof(T)));
        }
        try {
            std::uninitialized_copy(other.data(), other.data() + other.size(), data());
            size_ = other.size();
            capacity_ = other.capacity();
        } catch (...) {
            if (!is_using_static_buffer()) {
                ::operator delete(data_);
            }
            throw;
        }
    }

    template<typename T, size_t N>
    vector<T, N> &vector<T, N>::operator=(const vector &other) {
        vector tmp(other);
        swap(tmp);
        return *this;
    }

    template<typename T, size_t N>
    vector<T, N>::vector(vector &&other) : storage_{}, data_(reinterpret_cast<T *>(storage_)), size_{0}, capacity_{N} {
        swap(other);
    }

    template<typename T, size_t N>
    vector<T, N> &vector<T, N>::operator=(vector &&other) {
        swap(other);
        other.clear();
        return *this;
    }

    template<typename T, size_t N>
    vector<T, N>::~vector() {
        clear();
        if (!is_using_static_buffer()) {
            ::operator delete(data_);
        }
    }

    template<typename T, size_t N>
    T *vector<T, N>::data() {
        return data_;
    }

    template<typename T, size_t N>
    const T *vector<T, N>::data() const {
        return data_;
    }

    template<typename T, size_t N>
    T &vector<T, N>::operator[](size_t i) {
        return data()[i];
    }

    template<typename T, size_t N>
    const T &vector<T, N>::operator[](size_t i) const {
        return data()[i];
    }

    template<typename T, size_t N>
    void vector<T, N>::clear() {
        std::destroy_n(data(), size());
        size_ = 0;
    }

    template<typename T, size_t N>
    void vector<T, N>::reserve(const size_t new_capacity) {
        if (new_capacity <= capacity()) {
            return;
        }

        T *new_data = static_cast<T *>(::operator new(sizeof(T) * new_capacity));

        size_t i;
        try {
            for (i = 0; i < size(); i++)
                new(new_data + i) T(std::move_if_noexcept(*(data_ + i)));
        } catch (...) {
            std::destroy_n(new_data, i);
            ::operator delete(new_data);
            throw;
        }

        if (!is_using_static_buffer()) {
            ::operator delete(data_);
        } else {
            std::destroy_n(reinterpret_cast<T *>(storage_), size());
        }

        data_ = new_data;
        capacity_ = new_capacity;
    }

    template<typename T, size_t N>
    void vector<T, N>::push_back(const T &value) {
        if (size() == capacity()) {
            reserve(capacity() * 2);
        }
        std::construct_at(data() + size(), value);
        size_++;
    }

    template<typename T, size_t N>
    void vector<T, N>::push_back(T &&value) {
        if (size() == capacity()) {
            reserve(capacity() * 2);
        }
        std::construct_at(data() + size(), std::move(value));
        size_++;
    }

    template<typename T, size_t N>
    template<typename... Ts>
    void vector<T, N>::emplace_back(Ts &&... params) {
        if (size() == capacity()) {
            reserve(capacity() * 2);
        }
        std::construct_at(data() + size(), std::forward<Ts>(params)...);
        size_++;
    }

    template<typename T, size_t N>
    void vector<T, N>::pop_back() {
        if (size() != 0) {
            size_--;
            std::destroy_at(data() + size());
        }
    }

    template<typename T, size_t N>
    size_t vector<T, N>::capacity() const {
        return capacity_;
    }

    template<typename T, size_t N>
    size_t vector<T, N>::size() const {
        return size_;
    }

    template<typename T, size_t N>
    void vector<T, N>::swap_between_static_buffers(vector &other) {
        if (size_ > other.size_) {
            std::swap_ranges(data(), data() + other.size_, other.data());
            std::uninitialized_move(data() + other.size_, data() + size_, other.data() + other.size_);
            std::destroy(data() + other.size_, data() + size_);
        } else if (size_ < other.size_) {
            std::swap_ranges(data(), data() + size_, other.data());
            std::uninitialized_move(other.data() + size_, other.data() + other.size_, data() + size_);
            std::destroy(other.data() + size_, other.data() + other.size_);
        } else {
            std::swap_ranges(data(), data() + size_, other.data());
        }
    }

    template<typename T, size_t N>
    void vector<T, N>::swap_dynamic_with_static(vector &dynamic_, vector &static_) {
        std::swap_ranges(static_.data(), static_.data() + static_.size(), dynamic_.data());
        const size_t original_size = static_.size();
        for (size_t i = original_size; i < dynamic_.size(); ++i) {
            static_.push_back(dynamic_[i]);
        }
        std::destroy(dynamic_.data() + original_size, dynamic_.data() + dynamic_.size_);
        static_.size_ = original_size;
    }

    template<typename T, size_t N>
    bool vector<T, N>::is_using_static_buffer() {
        return data_ == reinterpret_cast<T *>(storage_);
    }

    template<typename T, size_t N>
    void vector<T, N>::swap(vector &other) {
        try {
            if (is_using_static_buffer()) {
                if (other.is_using_static_buffer()) {
                    swap_between_static_buffers(other);
                } else {
                    swap_dynamic_with_static(other, *this);
                }
            } else if (other.is_using_static_buffer()) {
                swap_dynamic_with_static(*this, other);
            } else {
                std::swap(data_, other.data_);
            }
            std::swap(size_, other.size_);
            std::swap(capacity_, other.capacity_);
        } catch (...) {
            throw;
        }
    }
}
