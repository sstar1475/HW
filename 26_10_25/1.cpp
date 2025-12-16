//
// 03.06
//

#include <cmath>
#include <iostream>
#include <vector>

double scalarProduct(const std::pair<double, double> &v1, const std::pair<double, double> &v2) {
    return v1.first * v2.first + v1.second * v2.second;
}

double vectorProduct(const std::pair<double, double> &v1, const std::pair<double, double> &v2) {
    return v1.first * v2.second - v1.second * v2.first;
}

class Shape {
public:
    virtual ~Shape() = default;
    [[nodiscard]] virtual double perimeter() const = 0;
    [[nodiscard]] virtual double area() const = 0;
};

class Triangle : public Shape {
public:
    explicit Triangle(const std::pair<double, double> &point, const std::pair<double, double> &v1,
                      const std::pair<double, double> &v2) : point(point), v1_(v1), v2_(v2) {
        v3_.first = -v1_.first + v2_.first;
        v3_.second = -v1_.second + v2_.second;
    }

    // [[nodiscard]] double perimeter() const override final { return 0; }
    [[nodiscard]] double perimeter() const final {
        return std::sqrt(scalarProduct(v1_, v1_)) + std::sqrt(scalarProduct(v2_, v2_)) +
               std::sqrt(scalarProduct(v3_, v3_));
    }

    [[nodiscard]] double area() const final { return std::abs(vectorProduct(v1_, v2_)) / 2; }

private:
    std::pair<double, double> point = {0, 0};
    std::pair<double, double> v1_;
    std::pair<double, double> v2_;
    std::pair<double, double> v3_;
};

class Square final : public Shape {
public:
    explicit Square(const std::pair<double, double> &point, const std::pair<double, double> &v1) :
        point_(point), v1_(v1) {}

    [[nodiscard]] double perimeter() const override { return 4 * std::sqrt(scalarProduct(v1_, v1_)); }

    [[nodiscard]] double area() const override { return scalarProduct(v1_, v1_); }


private:
    std::pair<double, double> point_ = {0, 0};
    std::pair<double, double> v1_;
};

class Circle final : public Shape {
public:
    explicit Circle(const std::pair<double, double> &center, double radius) : center_(center), radius_(radius) {}

    [[nodiscard]] double perimeter() const override { return 2 * radius_ * M_PI; }

    [[nodiscard]] double area() const override { return M_PI * radius_ * radius_; }

private:
    std::pair<double, double> center_ = {0, 0};
    double radius_;
};

int main() {
    std::pair<double, double> begin(2, -5);
    std::pair<double, double> v1(4, 0);
    std::pair<double, double> v2(0, 3);
    double radius = 5;
    Triangle triangle(begin, v1, v2);
    Square square(begin, v2);
    Circle circle(begin, radius);
    std::vector<Shape *> shapes = {&triangle, &square, &circle};
    for (auto shape: shapes) {
        std::cout << "Shape type is " << (typeid(*shape).name() + 1) << '\n';
        std::cout << "Shape perimeter is " << shape->perimeter() << '\n';
        std::cout << "Shape area is " << shape->area() << '\n';
        std::cout << "---\n";
    }
    return 0;
}
