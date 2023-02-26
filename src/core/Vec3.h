#pragma once

template <class T>
class Vec3 {
 public:
  /// @brief The x component
  T x;

  /// @brief The y component
  T y;

  /// @brief The z component
  T z;

  /**
   * @brief Construct a new Vec3 object
   * @details Creates a new Vec3 object with the given x, y and z components
   *
   * @param x - The x component
   * @param y - The y component
   * @param z - The z component
   */
  Vec3(T x, T y, T z) : x(x), y(y), z(z) {}

  /**
   * @brief Construct a new Vec3 object
   * @details Creates a new Vec3 object with the x, y and z components set to 0
   *
   */
  Vec3() : x(0), y(0), z(0) {}

  /**
   * @brief Construct a new Vec3 object from another Vec3 object
   *
   * @param other - The other Vec3 object
   */
  Vec3(const Vec3& other) : x(other.x), y(other.y), z(other.z) {}

  /**
   * @brief Copy assignment operator
   *
   * @param other - The other Vec3 object
   * @return Vec3&
   */
  Vec3& operator=(const Vec3& other) {
    x = other.x;
    y = other.y;
    z = other.z;
    return *this;
  }

  /**
   * @brief Addition assignment operator
   *
   * @param other - The other Vec3 object
   * @return Vec3&
   */
  Vec3& operator+=(const Vec3& other) {
    x += other.x;
    y += other.y;
    z += other.z;
    return *this;
  }

  /**
   * @brief Subtraction assignment operator
   *
   * @param other - The other Vec3 object
   * @return Vec3&
   */
  Vec3& operator-=(const Vec3& other) {
    x -= other.x;
    y -= other.y;
    z -= other.z;
    return *this;
  }

  /**
   * @brief Multiplication assignment operator
   *
   * @param other - The other Vec3 object
   * @return Vec3&
   */
  Vec3& operator*=(const Vec3& other) {
    x *= other.x;
    y *= other.y;
    z *= other.z;
    return *this;
  }

  /**
   * @brief Division assignment operator
   *
   * @param other - The other Vec3 object
   * @return Vec3&
   */
  Vec3& operator/=(const Vec3& other) {
    x /= other.x;
    y /= other.y;
    z /= other.z;
    return *this;
  }

  /**
   * @brief Addition operator
   *
   * @param other - The other Vec3 object
   * @return Vec3&
   */
  Vec3& operator+(const Vec3& other) {
    return Vec3(x + other.x, y + other.y, z + other.z);
  }

  /**
   * @brief Subtraction operator
   *
   * @param other - The other Vec3 object
   * @return Vec3&
   */
  Vec3& operator-(const Vec3& other) {
    return Vec3(x - other.x, y - other.y, z - other.z);
  }

  /**
   * @brief Multiplication operator
   *
   * @param other - The other Vec3 object
   * @return Vec3&
   */
  Vec3& operator*(const Vec3& other) {
    return Vec3(x * other.x, y * other.y, z * other.z);
  }

  /**
   * @brief Division operator
   *
   * @param other - The other Vec3 object
   * @return Vec3&
   */
  Vec3& operator/(const Vec3& other) {
    return Vec3(x / other.x, y / other.y, z / other.z);
  }

  /**
   * @brief Equality operator
   *
   * @param other - The other Vec3 object
   * @return Vec3&
   */
  bool operator==(const Vec3& other) {
    return x == other.x && y == other.y && z == other.z;
  }

  /**
   * @brief Inequality operator
   *
   * @param other - The other Vec3 object
   * @return Vec3&
   */
  bool operator!=(const Vec3& other) {
    return x != other.x || y != other.y || z != other.z;
  }
};
