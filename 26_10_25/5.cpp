#include <algorithm>
#include <cassert>
#include <iostream>
#include <vector>

class Vector {
private:
    struct DataHolder {
        size_t size;
        int ref_count;
        double *data;

        explicit DataHolder(size_t s) : size(s), ref_count(1), data(new double[s]) { std::fill_n(data, size, 0.0); }

        DataHolder(const double *src, size_t s) : size(s), ref_count(1) {
            data = new double[size];
            std::copy_n(src, size, data);
        }

        DataHolder(const DataHolder &) = delete;
        DataHolder &operator=(const DataHolder &) = delete;

        ~DataHolder() { delete[] data; }
    };

    DataHolder *holder_ = nullptr;

    mutable bool is_const_buffer_acquired_ = false;
    bool is_buffer_acquired_ = false;

    void detach() {
        if (holder_ == nullptr)
            return;
        if (holder_->ref_count > 1) {
            --holder_->ref_count;
            holder_ = new DataHolder(holder_->data, holder_->size);
        }
    }

    void release_holder() {
        if (holder_ == nullptr)
            return;
        holder_->ref_count--;
        if (holder_->ref_count == 0)
            delete holder_;
        holder_ = nullptr;
    }

public:
    explicit Vector(size_t size) {
        if (size > 0)
            holder_ = new DataHolder(size);
    }

    Vector(const Vector &other) : holder_(other.holder_) {
        if (holder_ != nullptr)
            ++holder_->ref_count;
        if (other.is_buffer_acquired_)
            detach();
    }

    Vector &operator=(const Vector &other) {
        if (this == &other)
            return *this;
        release_holder();
        holder_ = other.holder_;
        if (holder_ != nullptr)
            holder_->ref_count++;
        if (other.is_buffer_acquired_)
            detach();
        is_const_buffer_acquired_ = false;
        is_buffer_acquired_ = false;
        return *this;
    }

    Vector(const Vector &other, size_t offset, size_t size) {
        holder_ = other.holder_ != nullptr ? new DataHolder(other.holder_->data + offset, size) : nullptr;
    }

    ~Vector() { release_holder(); }

    double &operator[](size_t index) {
        assert(!is_const_buffer_acquired_ && "Cannot modify Vector while const buffer is acquired");
        assert(!is_buffer_acquired_ && "Cannot modify Vector while buffer is acquired");
        detach();
        return holder_->data[index];
    }

    const double &operator[](size_t index) const {
        return holder_->data[index];
    }

    size_t size() const { return holder_ ? holder_->size : 0; }

    double *AcquireBuffer() {
        assert(!is_buffer_acquired_ && "Buffer is already acquired");
        assert(!is_const_buffer_acquired_ && "Const buffer is already acquired");
        detach();
        is_buffer_acquired_ = true;
        return holder_ ? holder_->data : nullptr;
    }

    void ReleaseBuffer() {
        assert(is_buffer_acquired_ && "Buffer was not acquired");
        is_buffer_acquired_ = false;
    }

    const double *AcquireConstBuffer() const {
        assert(!is_buffer_acquired_ && "Buffer is already acquired");
        assert(!is_const_buffer_acquired_ && "Const buffer is already acquired");
        is_const_buffer_acquired_ = true;
        return holder_ ? holder_->data : nullptr;
    }

    void ReleaseConstBuffer() const {
        assert(is_const_buffer_acquired_ && "Const buffer was not acquired");
        is_const_buffer_acquired_ = false;
    }

    class Buffer {
    private:
        Vector &vec_;
        double *raw_data_;

    public:
        explicit Buffer(Vector &v) : vec_(v) { raw_data_ = vec_.AcquireBuffer(); }

        ~Buffer() { vec_.ReleaseBuffer(); }

        [[nodiscard]] size_t size() const { return vec_.size(); }

        explicit operator double *() const { return raw_data_; }

        double *get() const { return raw_data_; }

        Buffer(const Buffer &) = delete;
        Buffer &operator=(const Buffer &) = delete;
    };

    class ConstBuffer {
    private:
        const Vector &vec_;
        const double *raw_data_;

    public:
        explicit ConstBuffer(const Vector &v) : vec_(v) { raw_data_ = vec_.AcquireConstBuffer(); }

        ~ConstBuffer() { vec_.ReleaseConstBuffer(); }

        [[nodiscard]] size_t size() const { return vec_.size(); }

        explicit operator const double *() const { return raw_data_; }

        const double *get() const { return raw_data_; }

        ConstBuffer(const ConstBuffer &) = delete;
        ConstBuffer &operator=(const ConstBuffer &) = delete;
    };
};

void Print(const Vector &v) {
    std::cout << "{";
    for (size_t i = 0; i < v.size(); ++i) {
        std::cout << v[i] << " , ";
    }
    std::cout << "}";
}

void testV() {
    Vector v1(5);
    Vector v2(v1);
    Vector v3 = v1;
    v1[0] = 1;
    std::cout << "v1[0] := 1" << std::endl;
    std::cout << "v1 = ";
    Print(v1);
    std::cout << std::endl;
    std::cout << "v2 = ";
    Print(v2);
    std::cout << std::endl;
    std::cout << "v3 = ";
    Print(v3);
    std::cout << std::endl;
    Vector v4(v1, 0, 3);
    std::cout << "v4 = ";
    Print(v4);
    std::cout << std::endl << std::endl;
}

void testBuffer() {
    Vector v1(5);
    Vector::Buffer b1(v1);
    // Vector::ConstBuffer b2(v1); // error
    Vector v2(v1);
    Vector::ConstBuffer cb2(v2);
    b1.get()[0] = 1;
    std::cout << "b1.get()[0] = " << b1.get()[0] << std::endl;
    std::cout << "v2 = ";
    Print(v2);
    std::cout << std::endl;
    // cb2.get(0) = 1; // error
    // v2[0] = 1; // error
}

int main() {
    testV();
    testBuffer();
    return 0;
}
