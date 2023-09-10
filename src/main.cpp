#include <algorithm>
#include <cmath>
#include <functional>
#include <iomanip>
#include <iostream>
#include <memory>
#include <numeric>
#include <random>
#include <string_view>
#include <vector>

#include <Curves.hpp>

using Circles = std::vector<std::shared_ptr<curves::Circle>>;
using Curves = std::vector<std::shared_ptr<curves::Curve>>;

Curves gen_curves(const std::size_t N) {
  std::mt19937 gen(std::random_device{}());

  std::uniform_real_distribution<double> dist(0.1, 42.);
  std::uniform_int_distribution<std::uint16_t> curve(0, 2);

  Curves curves;
  curves.reserve(N);

  auto generator = [&](auto &&f) -> std::shared_ptr<curves::Curve> {
    switch (f()) {
    case 0:
      return std::make_shared<curves::Circle>(dist(gen));
    case 1:
      return std::make_shared<curves::Ellipse>(dist(gen), dist(gen));
    default:
      return std::make_shared<curves::Helix>(dist(gen), dist(gen));
    }
  };

  std::generate_n(std::back_inserter(curves), 3,
                  std::bind(generator, [i = 0]() mutable { return i++; }));

  std::generate_n(std::back_inserter(curves), N - 3,
                  std::bind(generator, [&]() { return curve(gen); }));

  return curves;
}

Circles get_circles(const Curves &curves) {
  Circles circles;
  circles.reserve(std::size(curves));

  for (const auto &curve : curves) {
    if (auto &&circle = std::dynamic_pointer_cast<curves::Circle>(curve);
        circle) {
      circles.emplace_back(circle);
    }
  }

  std::sort(std::begin(circles), std::end(circles),
            [](const auto &lhs, const auto &rhs) {
              return lhs->get_radius() < rhs->get_radius();
            });

  return circles;
}

template <typename T>
void print_curves(std::string_view title,
                  const std::vector<std::shared_ptr<T>> &curves,
                  const double t) {
  std::cout << title << std::endl;

  for (const auto &curve : curves) {
    std::cout << std::left << std::setw(14) << curve->get_name()
              << " point = " << curve->get_point(t)
              << "; derivative = " << curve->get_first_derivative(t)
              << std::endl;
  }

  std::cout << std::endl;
}

double radius_sum(const Circles &circles) {
  return std::accumulate(
      std::cbegin(circles), std::cend(circles), 0.,
      [](double sum, const std::shared_ptr<curves::Circle> &circle) {
        return sum + circle->get_radius();
      });
}

int main() {
  constexpr std::size_t N = 100;

  const Curves curves = gen_curves(N);
  const Circles circles = get_circles(curves);

  constexpr double t = M_PI_4;

  print_curves("All curves", curves, t);
  print_curves("Sorted circles", circles, t);

  std::cout << "Circles radius_sum: " << radius_sum(circles) << std::endl;

  return 0;
}