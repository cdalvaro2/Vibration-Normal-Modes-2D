//
//  vector.hpp
//  Class for vectorial operations
//  Vibration Normal Modes 2D
//
//  Created by Carlos David on 5/8/12.
//  Copyright (c) 2012 cdalvaro. All rights reserved.
//

#pragma once

#include <iostream>
#include <iomanip>
#include <cmath>
#include <stdlib.h>
#include <fstream>
#include <algorithm>
#include <string>
#include <stdexcept>

//  Para QR
#define Qmatrix     0x01
#define Rmatrix     0x02
#define QRmatrix    0x04

//  Para mostrar las iteraciones de los autovalores y de los autovectores
#define EVa_Ite     0x01
#define EVe_Ite     0x02
#define ORGANIZED   0x04


namespace cda {
    namespace math {
        namespace containers {
    
            template <typename T>
            class Vector {
            private:
                size_t n;
                T *v, *it_end;
                
                void AllocateMemory(const size_t &size) {
                    if (void *mem = std::realloc(v, size * sizeof(T))) {
                        v = static_cast<T *>(mem);
                        it_end = v + size;
                    } else {
                        throw std::bad_alloc();
                    }
                }
                
            public:
                
                /**
                 Class constructor
                 
                 @param size The size of the vector
                 */
                Vector(const size_t &size = 0) :
                n(size), v(nullptr), it_end(nullptr) {
                    AllocateMemory(n);
                }
                
                /**
                 Class constructor
                 
                 @param size The size of the vector
                 */
                Vector(const size_t &size, const T &value) :
                n(size), v(nullptr), it_end(nullptr) {
                    AllocateMemory(n);
                    std::fill(this->Begin(), this->End(), value);
                }
                
                /**
                 Copy constructor
                 
                 @param vector The source vector
                 */
                Vector(const Vector<T> &vector) :
                n(vector.n), v(nullptr), it_end(nullptr) {
                    AllocateMemory(n);
                    std::copy(vector.Begin(), vector.End(), this->Begin());
                }
                
                /**
                 Move constructor
                 
                 @param vector The source vector
                 */
                Vector(Vector<T> &&vector) :
                n(vector.n), v(vector.v), it_end(vector.it_end) {
                    vector.n = 0;
                    vector.v = vector.it_end = nullptr;
                }
                
                /**
                 Class destructor
                 */
                ~Vector() {
                    if (v) {
                        std::free(v);
                        v = it_end = nullptr;
                        n = 0;
                    }
                }
                
                T *Begin() const {
                    return v;
                }
                
                T *End() const {
                    return it_end;
                }
                
                /**
                 Change the size of the vector
                 
                 @param size The new size of the vector
                 @param fill If true and size is bigger than the previous size
                 the new extra elements will be set to 0
                 */
                void Resize(const size_t &size, const bool &fill = true) {
                    if (size == n) {
                        return;
                    }
                    
                    AllocateMemory(size);
                    if (fill && size > n) {
                        std::fill_n(v + n, size - n, static_cast<T>(0));
                    }
                    
                    n = size;
                }
                
                Vector<T> &operator=(const Vector<T> &vector) {
                    if (this != &vector) {
                        Resize(vector.n, false);
                        std::copy(vector.Begin(), vector.End(), this->Begin());
                    }
                    return *this;
                }
                
                Vector<T> &operator=(Vector<T> &&vector) {
                    if (this != &vector) {
                        std::free(v);
                        v = vector.v;
                        it_end = vector.it_end;
                        n = vector.n;
                        
                        vector.v = vector.it_end = nullptr;
                        vector.n = 0;
                    }
                    return *this;
                }
                
                void Copy(const size_t &size, const T *array) {
                    if (size > n) {
                        throw std::out_of_range("Not enough space for the copy");
                    }
                    std::copy(array, array + size, v);
                }
                
                bool operator==(const Vector<T> &vector) const {
                    if (this->n != vector.n) {
                        return false;
                    }
                    
                    auto it_vector = vector.Begin();
                    for (auto it = this->Begin(); it != this->End(); ++it, ++it_vector) {
                        if (*it != *it_vector) {
                            return false;
                        }
                    }
                    
                    return true;
                }
                
                bool operator!=(const Vector<T> &vector) const {
                    return !this->operator==(vector);
                }
                
                /**
                 Returns \p elements starting from element \p first_element
                 
                 @param first_element The first element to be recovered
                 @param elements The number of elements to be recovered
                 
                 @return A vector with size \p elements and the values starting from \p first_element
                 */
                Vector<T> Get(const size_t &first_element, const size_t &elements) {
                    if (n - first_element < elements) {
                        throw std::out_of_range("There are not enough elements inside the vector");
                    }
                    
                    Vector<T> tmp(n - first_element);
                    std::copy(v + first_element, v + first_element + elements, tmp.v);
                    
                    return tmp;
                }
                
                /**
                 Returns a vector starting at position \p first_element
                 
                 @param first_element The position of the first element to be recovered
                 
                 @return A vector with all values starting from \p first_element
                 */
                Vector<T> Get(const size_t &first_element) {
                    return Get(first_element, n - first_element);
                }
                
                /**
                 Copy \p elements from \p vector starting at position \p first_element
                 
                 @param first_element The position of the first element to be copied
                 @param vector The vector with the elements to be copied
                 @param elements The number of elements to copy
                 */
                void Set(const size_t &first_element, const Vector<T> &vector, const size_t &elements) {
                    if (first_element + elements > n || elements > vector.n) {
                        throw std::out_of_range("Out of bounds");
                    }
                    std::copy(vector.v, vector.v + elements, v + first_element);
                }
                
                /**
                 Copy all elements of \p vector at position \p first_element
                 
                 @param first_element The position of the first element to be copied
                 @param vector The vector to copy
                 */
                void Set(const size_t &first_element, const Vector<T> &vector) {
                    Set(first_element, vector, vector.n);
                }
                
                size_t Size() const {
                    return n;
                }
                
                T MaximumElement() const {
                    auto max = *Begin();
                    for (auto it = Begin() + 1; it != End(); ++it) {
                        if (*it > max) {
                            max = *it;
                        }
                    }
                    return max;
                }
                
                T AbsoluteMaximumElement() const {
                    auto max = std::abs(*Begin());
                    T abs_max;
                    for (auto it = Begin() + 1; it != End(); ++it) {
                        abs_max = std::abs(*it);
                        if (abs_max > max) {
                            max = abs_max;
                        }
                    }
                    return max;
                }
                
                T MinimumElement() const {
                    auto min = *Begin();
                    for (auto it = Begin() +1 ; it != End(); ++it) {
                        if (*it < min) {
                            min = *it;
                        }
                    }
                    return min;
                }
                
                T AbsoluteMinimumElement() const {
                    auto min = std::abs(*Begin());
                    T abs_min;
                    for (auto it = Begin() + 1; it != End(); ++it) {
                        abs_min = std::abs(*it);
                        if (abs_min < min) {
                            min = abs_min;
                        }
                    }
                    return min;
                }
                
                T Sum() const {
                    auto sum = *Begin();
                    for (auto it = Begin() + 1; it != End(); ++it) {
                        sum += *it;
                    }
                    return sum;
                }
                
                const T &At(const size_t &element) const {
                    if (element >= n) {
                        throw std::out_of_range("Index out of bounds");
                    }
                    return v[element];
                }
                
                const T &operator[](const size_t &element) const {
                    return v[element];
                }
                
                T &At(const size_t &element) {
                    if (element >= n) {
                        throw std::out_of_range("Index out of bounds");
                    }
                    return v[element];
                }
                
                T &operator[](const size_t &element) {
                    return v[element];
                }
                
                T Norm() const {
                    return std::sqrt(SquaredNorm());
                }
                
                T SquaredNorm() const {
                    return *this * *this;
                }
                
                /**
                 Returns a normalized vector
                 
                 @return The normalized vector
                 */
                Vector<T> Unitary() const {
                    return (* this) / Norm();
                }
                
                void Sort() {
                    bool change;
                    do {
                        change = false;
                        for (auto it_prev = Begin(), it = it_prev + 1; it != End(); ++it_prev, ++it) {
                            if (*it_prev > *it) {
                                change = true;
                                std::swap(*it, *it_prev);
                            }
                        }
                    } while (change);
                }
                
                void Fill(const T &value) {
                    std::fill(Begin(), End(), value);
                }
                
                static Vector<T> Zero(const size_t &size) {
                    return Vector<T>(size, 0);
                }
                
                void Zero() {
                    Fill(0);
                }
                
                static Vector<T> Ones(const size_t &size) {
                    return Vector<T>(size, 1);
                }
                
                void Ones() {
                    Fill(1);
                }
                
                static Vector<T> Random(const size_t &size) {
                    Vector<T> random(size);
                    random.Random();
                    return random;
                }
                
                void Random(const T &min = 0, const T &max = 1) {
                    for (auto it = Begin(); it != End(); ++it) {
                        *it = static_cast<T>(drand48() * max + min);
                    }
                }
                
                bool IsNull() const {
                    for (auto it = Begin(); it != End(); ++it) {
                        if (*it != 0) {
                            return false;
                        }
                    }
                    return true;
                }
                
                bool HasDuplicated(const T &precision) const {
                    T distance;
                    for (auto it1 = Begin(); it1 != End(); ++it1) {
                        for (auto it2 = Begin(); it2 != End(); ++it2) {
                            if (it1 != it2) {
                                distance = *it1 - *it2;
                                if (std::sqrt(distance * distance) < precision) {
                                    return true;
                                }
                            }
                        }
                    }
                    return false;
                }
                
                bool HasDuplicated() const {
                    for (auto it1 = Begin(); it1 != End(); ++it1) {
                        for (auto it2 = Begin(); it2 != End(); ++it2) {
                            if (it1 != it2 && *it1 == *it2) {
                                return true;
                            }
                        }
                    }
                    return false;
                }
                
                void Write(const std::string &filename, const std::string &path = getenv("HOME")) const {
                    const std::string file_path(path + filename);
                    std::ofstream output(file_path.data());
                    output << *this;
                }
                
                //  --- OPERATORS ---
                inline Vector<T> operator+(const Vector<T> &vector) const {
                    if (this->n != vector.n) {
                        throw std::logic_error("Unable to sum two vector of different size");
                    }
                    
                    Vector<T> new_vector(this->n);
                    
                    auto it_new = new_vector.Begin();
                    auto it_vector = vector.Begin();
                    for (auto it_this = this->Begin(); it_this != this->End(); ++it_this, ++it_vector, ++it_new) {
                        *it_new = *it_this + *it_vector;
                    }
                    
                    return new_vector;
                }
                
                inline Vector<T> &operator+=(const Vector<T> &vector) {
                    if (this->n != vector.n) {
                        throw std::logic_error("Unable to sum two vector of different size");
                    }
                    
                    auto it_vector = vector.Begin();
                    for (auto it_this = this->Begin(); it_this != this->End(); ++it_this, ++it_vector) {
                        *it_this += *it_vector;
                    }
                    
                    return *this;
                }
                
                inline Vector<T> operator-(const Vector<T> &vector) const {
                    if (this->n != vector.n) {
                        throw std::logic_error("Unable to sum two vector of different sizes");
                    }
                    
                    Vector<T> new_vector(this->n);
                    
                    auto it_new = new_vector.Begin();
                    auto it_vector = vector.Begin();
                    for (auto it_this = this->Begin(); it_this != this->End(); ++it_this, ++it_vector, ++it_new) {
                        *it_new = *it_this - *it_vector;
                    }
                    
                    return new_vector;
                }
                
                inline Vector<T> &operator-=(const Vector<T> &vector) {
                    if (this->n != vector.n) {
                        throw std::logic_error("Unable to sum two vector of different size");
                    }
                    
                    auto it_vector = vector.Begin();
                    for (auto it_this = this->Begin(); it_this != this->End(); ++it_this, ++it_vector) {
                        *it_this -= *it_vector;
                    }
                    
                    return *this;
                }
                
                inline Vector<T> operator*(const T &value) const {
                    Vector<T> new_vector(this->n);
                    auto it_new = new_vector.Begin();
                    
                    for (auto it_this = this->Begin(); it_this != this->End(); ++it_this, ++it_new) {
                        *it_new = *it_this * value;
                    }
                    
                    return new_vector;
                }
                
                inline Vector<T> &operator*=(const T &value) {
                    for (auto it = this->Begin(); it != this->End(); ++it) {
                        *it *= value;
                    }
                    return *this;
                }
                
                inline Vector<T> operator/(const T &value) const {
                    Vector<T> new_vector(this->n);
                    auto it_new = new_vector.Begin();
                    
                    for (auto it = this->Begin(); it != this->End(); ++it, ++it_new) {
                        *it_new = *it / value;
                    }
                    
                    return new_vector;
                }
                
                inline Vector<T> &operator/=(const T &value) {
                    for (auto it = this->Begin(); it != this->End(); ++it) {
                        *it /= value;
                    }
                    return *this;
                }
                
                template<typename Integer,
                         typename = std::enable_if<std::is_integral<Integer>::value>>
                inline Vector<T> operator%(const Integer &value) const {
                    Vector<Integer> new_vector(this->n);
                    auto it_new = new_vector.Begin();
                    
                    for (auto it = this->Begin(); it != this->End(); ++it, ++it_new) {
                        *it_new = static_cast<Integer>(*it) % value;
                    }
                    
                    return new_vector;
                }
                
                template<typename Integer,
                         typename = std::enable_if<std::is_integral<Integer>::value>>
                inline Vector<T> &operator%=(const Integer& value) {
                    for (auto it = this->Begin(); it != this->End(); ++it) {
                        *it = static_cast<Integer>(*it) % value;
                    }
                    return *this;
                }
                
                inline Vector<T> operator-() const {
                    Vector<T> new_vector(this->n);
                    auto it_new = new_vector.Begin();
                    
                    for (auto it = this->Begin(); it != this->End(); ++it, ++it_new) {
                        *it_new = -(*it);
                    }
                    
                    return new_vector;
                }
                
                Vector<T> CrossProduct3D(const Vector<T> &vector) const {
                    if (this->n != vector.n || this->n != 3) {
                        throw std::logic_error("Both vectors must be of the same size, and size must be 3");
                    }
                    
                    Vector<T> new_vector(this->n);
                    new_vector[0] = this->operator[](1) * vector[2] - this->operator[](2) * vector[1];
                    new_vector[1] = this->operator[](2) * vector[0] - this->operator[](0) * vector[2];
                    new_vector[2] = this->operator[](0) * vector[1] - this->operator[](1) * vector[0];
                    
                    return new_vector;
                }
                
                inline T operator*(const Vector<T> &vector) const {
                    if (this->n != vector.n) {
                        throw std::logic_error("Both vectors must be of the same size");
                    }
                    
                    T value(0);
                    auto it_vector = vector.Begin();
                    
                    for (auto it_this = this->Begin(); it_this != this->End(); ++it_this, ++it_vector) {
                        value += *it_this * *it_vector;
                    }
                    
                    return value;
                }
                
            };
    
        } /* namespace math */
    } /* namespace containers */
} /* namespace containers */

//  --- MORE OPERATORS ---
template <typename T, typename T2>
inline static cda::math::containers::Vector<T> operator*(const T2 &value, const cda::math::containers::Vector<T> &vector) {
    cda::math::containers::Vector<T> new_vector(vector.Size());
    auto it_new = new_vector.Begin();
    
    for (auto it = vector.Begin(); it != vector.End(); ++it, ++it_new) {
        *it_new = *it * static_cast<T>(value);
    }
    
    return new_vector;
}
