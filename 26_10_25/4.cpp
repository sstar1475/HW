#include <array>
#include <cstdint>
#include <iostream>
#include <sstream>
#include <string>

class IPv4 {
private:
    std::array<std::uint8_t, 4> data;

public:
    IPv4() : data{0, 0, 0, 0} {}

    IPv4(std::uint8_t a, std::uint8_t b, std::uint8_t c, std::uint8_t d) : data{a, b, c, d} {}

    IPv4 &operator++() {
        for (int i = 3; i >= 0; --i) {
            if (data[i] < 255) {
                data[i]++;
                return *this;
            }
            data[i] = 0;
        }
        return *this;
    }

    IPv4 operator++(int) {
        IPv4 copy = *this;
        ++(*this);
        return copy;
    }

    IPv4 &operator--() {
        for (int i = 3; i >= 0; --i) {
            if (data[i] > 0) {
                data[i]--;
                return *this;
            }
            data[i] = 255;
        }
        return *this;
    }

    IPv4 operator--(int) {
        IPv4 copy = *this;
        --(*this);
        return copy;
    }

    friend bool operator==(const IPv4 &lhs, const IPv4 &rhs) { return lhs.data == rhs.data; }

    friend bool operator!=(const IPv4 &lhs, const IPv4 &rhs) { return !(lhs == rhs); }

    friend bool operator<(const IPv4 &lhs, const IPv4 &rhs) { return lhs.data < rhs.data; }

    friend bool operator>(const IPv4 &lhs, const IPv4 &rhs) { return rhs < lhs; }

    friend bool operator<=(const IPv4 &lhs, const IPv4 &rhs) { return rhs >= lhs; }

    friend bool operator>=(const IPv4 &lhs, const IPv4 &rhs) { return !(lhs < rhs); }

    friend std::ostream &operator<<(std::ostream &os, const IPv4 &ip);
    friend std::istream &operator>>(std::istream &is, IPv4 &ip);
};

std::ostream &operator<<(std::ostream &os, const IPv4 &ip) {
    for (int i = 0; i < 4; ++i) {
        os << (i > 0 ? "." : "") << static_cast<int>(ip.data[i]);
    }
    return os;
}

std::istream &operator>>(std::istream &is, IPv4 &ip) {
    int parts[4];
    char temp_char;
    for (int i = 0; i < 4; ++i) {
        if (i > 0) {
            if (!(is >> temp_char) || temp_char != '.') {
                is.setstate(std::ios::failbit);
                return is;
            }
        }
        if (!(is >> parts[i]) || !(0 <= parts[i] && parts[i] <= 255)) {
            is.setstate(std::ios::failbit);
            return is;
        }
    }
    for (int i = 0; i < 4; ++i) {
        ip.data[i] = static_cast<uint8_t>(parts[i]);
    }
    return is;
}

int main() {
    IPv4 ip1;

    std::string input_str = "0.208.129.254";
    std::stringstream ss_in(input_str);

    ss_in >> ip1;

    std::cout << "Read: " << ip1 << std::endl;

    std::cout << "Original: " << ip1 << std::endl;
    std::cout << "ip++ = " << (ip1++) << std::endl;
    std::cout << "++ip = " << (++ip1) << std::endl;

    std::cout << std::endl;

    IPv4 ip2(0, 0, 0, 1);
    std::cout << "New IP: " << ip2 << std::endl;
    std::cout << "--ip = " << (--ip2) << std::endl;
    std::cout << "ip-- = " << (ip2--) << std::endl;

    std::cout << std::endl;

    IPv4 a(10, 0, 0, 1);
    IPv4 b(10, 0, 0, 2);

    if (a < b)
        std::cout << a << " < " << b << std::endl;
    if (b > a)
        std::cout << b << " > " << a << std::endl;

    std::stringstream ss_out;
    ss_out << a;
    std::string result_str = ss_out.str();
    std::cout << "Written: " << result_str << std::endl;

    return 0;
}
