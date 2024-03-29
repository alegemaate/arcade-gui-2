#pragma once

template <class T>
class Vec2 {
 public:
  /// @brief The x component
  T x;

  /// @brief The y component
  T y;

  /**
   * @brief Construct a new Vec2 object
   * @details Creates a new Vec2 object with the given x and y components
   *
   * @param x - The x component
   * @param y - The y component
   */
  Vec2(T x, T y) : x(x), y(y) {}

  /**
   * @brief Construct a new Vec 2 object
   * @details Creates a new Vec2 object with the x and y components set to 0
   *
   */
  Vec2() : x(0), y(0) {}

  /**
   * @brief Construct a new Vec 2 object from another Vec2 object
   *
   * @param other - The other Vec2 object
   */
  Vec2(const Vec2& other) : x(other.x), y(other.y) {}

  /**
   * @brief Copy assignment operator
   *
   * @param other - The other Vec2 object
   * @return Vec2&
   */
  Vec2& operator=(const Vec2& other) {
    x = other.x;
    y = other.y;
    return *this;
  }

  /**
   * @brief Addition assignment operator
   *
   * @param other - The other Vec2 object
   * @return Vec2&
   */
  Vec2& operator+=(const Vec2& other) {
    x += other.x;
    y += other.y;
    return *this;
  }

  /**
   * @brief Subtraction assignment operator
   *
   * @param other - The other Vec2 object
   * @return Vec2&
   */
  Vec2& operator-=(const Vec2& other) {
    x -= other.x;
    y -= other.y;
    return *this;
  }

  /**
   * @brief Multiplication assignment operator
   *
   * @param other - The other Vec2 object
   * @return Vec2&
   */
  Vec2& operator*=(const Vec2& other) {
    x *= other.x;
    y *= other.y;
    return *this;
  }

  /**
   * @brief Division assignment operator
   *
   * @param other - The other Vec2 object
   * @return Vec2&
   */
  Vec2& operator/=(const Vec2& other) {
    x /= other.x;
    y /= other.y;
    return *this;
  }

  /**
   * @brief Addition operator
   *
   * @param other - The other Vec2 object
   * @return Vec2&
   */
  Vec2& operator+(const Vec2& other) { return Vec2(x + other.x, y + other.y); }

  /**
   * @brief Subtraction operator
   *
   * @param other - The other Vec2 object
   * @return Vec2&
   */
  Vec2& operator-(const Vec2& other) { return Vec2(x - other.x, y - other.y); }

  /**
   * @brief Multiplication operator
   *
   * @param other - The other Vec2 object
   * @return Vec2&
   */
  Vec2& operator*(const Vec2& other) { return Vec2(x * other.x, y * other.y); }

  /**
   * @brief Division operator
   *
   * @param other - The other Vec2 object
   * @return Vec2&
   */
  Vec2& operator/(const Vec2& other) { return Vec2(x / other.x, y / other.y); }

  /**
   * @brief Equality operator
   *
   * @param other - The other Vec2 object
   * @return Vec2&
   */
  bool operator==(const Vec2& other) { return x == other.x && y == other.y; }

  /**
   * @brief Inequality operator
   *
   * @param other - The other Vec2 object
   * @return Vec2&
   */
  bool operator!=(const Vec2& other) { return x != other.x || y != other.y; }
};
