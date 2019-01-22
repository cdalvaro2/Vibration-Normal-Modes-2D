//
//  vector.hpp
//  Class for vectorial operations
//  Vibration Normal Modes 2D
//
//  Created by Carlos David on 5/8/12.
//  Copyright (c) 2012 cdalvaro. All rights reserved.
//

#pragma once

#include <algorithm>
#include <cmath>
#include <fstream>
#include <iostream>
#include <sstream>
#include <stdexcept>

#include "../algorithms/find.hpp"


namespace cda {
    namespace math {
        namespace containers {
    
            template <typename T>
            class Vector {
            private:
                size_t n;
                T *v, *it_end;
                
                void AllocateMemory(const size_t &size) {
                    if (size == 0) {
                        std::free(v);
                        v = it_end = nullptr;
                    } else {
                        if (void *mem = std::realloc(v, size * sizeof(T))) {
                            v = static_cast<T *>(mem);
                            it_end = v + size;
                        } else {
                            throw std::bad_alloc();
                        }
                    }
                }
                
            public:
                
                typedef T ValueType;
                
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
                Vector(const size_t &size, const ValueType &value) :
                n(size), v(nullptr), it_end(nullptr) {
                    AllocateMemory(n);
                    Fill(value);
                }
                
                /**
                 Copy constructor
                 
                 @param vector The source vector
                 */
                Vector(const Vector<ValueType> &vector) :
                n(vector.n), v(nullptr), it_end(nullptr) {
                    AllocateMemory(n);
                    std::copy(vector.begin(), vector.end(), this->begin());
                }
                
                /**
                 Move constructor
                 
                 @param vector The source vector
                 */
                Vector(Vector<ValueType> &&vector) :
                n(vector.n), v(vector.v), it_end(vector.it_end) {
                    vector.n = 0;
                    vector.v = vector.it_end = nullptr;
                }
                
                template <size_t size>
                Vector(const ValueType (& values)[size]) :
                n(size), v(nullptr), it_end(nullptr) {
                    AllocateMemory(size);
                    std::copy(values, values + size, this->begin());
                }
                
                Vector(const ValueType* const it_begin, const ValueType* const it_end) :
                n(std::distance(it_begin, it_end)), v(nullptr), it_end(nullptr) {
                    AllocateMemory(n);
                    std::copy(it_begin, it_end, this->begin());
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
                
                ValueType *begin() const {
                    return v;
                }
                
                ValueType *end() const {
                    return it_end;
                }
                
                /**
                 Change the size of the vector
                 
                 @param size The new size of the vector
                 @param fill If true and size is bigger than the previous size
                 the new extra elements will be set to 0
                 */
                void resize(const size_t &size, const bool &fill = true) {
                    if (size == n) {
                        return;
                    }
                    
                    AllocateMemory(size);
                    if (fill && size > n) {
                        std::fill_n(v + n, size - n, static_cast<ValueType>(0));
                    }
                    
                    n = size;
                }
                
                Vector<ValueType> &operator=(const Vector<ValueType> &vector) {
                    if (this != &vector) {
                        resize(vector.n, false);
                        std::copy(vector.begin(), vector.end(), this->begin());
                    }
                    return *this;
                }
                
                Vector<ValueType> &operator=(Vector<ValueType> &&vector) {
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
                
                void copy(const size_t &size, const ValueType* const array) {
                    resize(size);
                    std::copy(array, array + size, v);
                }
                
                bool operator==(const Vector<ValueType> &vector) const {
                    if (this->n != vector.n) {
                        return false;
                    }
                    
                    auto it_vector = vector.begin();
                    for (auto it = this->begin(); it != this->end(); ++it, ++it_vector) {
                        if (*it != *it_vector) {
                            return false;
                        }
                    }
                    
                    return true;
                }
                
                bool operator!=(const Vector<ValueType> &vector) const {
                    return !this->operator==(vector);
                }
                
                /**
                 Returns \p elements starting from element \p first_element
                 
                 @param first_element The first element to be recovered
                 @param elements The number of elements to be recovered
                 
                 @return A vector with size \p elements and the values starting from \p first_element
                 */
                Vector<ValueType> Get(const size_t &first_element, const size_t &elements) const {
                    if (n - first_element < elements) {
                        throw std::out_of_range("There are not enough elements inside the vector");
                    }
                    
                    auto it_begin = v + first_element;
                    auto it_end = it_begin + elements;
                    
                    return Vector<ValueType>(it_begin, it_end);
                }
                
                /**
                 Returns a vector starting at position \p first_element
                 
                 @param first_element The position of the first element to be recovered
                 
                 @return A vector with all values starting from \p first_element
                 */
                Vector<ValueType> Get(const size_t &first_element) const {
                    return Get(first_element, n - first_element);
                }
                
                /**
                 Copy \p elements from \p vector starting at position \p first_element
                 
                 @param first_element The position of the first element to be copied
                 @param vector The vector with the elements to be copied
                 @param elements The number of elements to copy
                 */
                void Set(const size_t &first_element, const Vector<ValueType> &vector, const size_t &elements) {
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
                void Set(const size_t &first_element, const Vector<ValueType> &vector) {
                    Set(first_element, vector, vector.n);
                }
                
                size_t Size() const {
                    return n;
                }
                
                ValueType MaximumElement() const {
                    return algorithms::find::MaximumElement(begin(), end());
                }
                
                ValueType AbsoluteMaximumElement() const {
                    return algorithms::find::AbsoluteMaximumElement(begin(), end());
                }
                
                ValueType AbsoluteMaximumElementWithSign() const {
                    return algorithms::find::AbsoluteMaximumElementWithSign(begin(), end());
                }
                
                ValueType MinimumElement() const {
                    return algorithms::find::MinimumElement(begin(), end());
                }
                
                ValueType AbsoluteMinimumElement() const {
                    return algorithms::find::AbsoluteMinimumElement(begin(), end());
                }
                
                ValueType AbsoluteMinimumElementWithSign() const {
                    return algorithms::find::AbsoluteMinimumElementWithSign(begin(), end());
                }
                
                ValueType SumAllEments() const {
                    auto sum = *begin();
                    for (auto it = begin() + 1; it != end(); ++it) {
                        sum += *it;
                    }
                    return sum;
                }
                
                const ValueType &At(const size_t &element) const {
                    if (element >= n) {
                        throw std::out_of_range("Index out of bounds");
                    }
                    return v[element];
                }
                
                const ValueType &operator[](const size_t &element) const {
                    return v[element];
                }
                
                ValueType &At(const size_t &element) {
                    if (element >= n) {
                        throw std::out_of_range("Index out of bounds");
                    }
                    return v[element];
                }
                
                ValueType &operator[](const size_t &element) {
                    return v[element];
                }
                
                double Norm() const {
                    return std::sqrt(SquareNorm());
                }
                
                double SquareNorm() const {
                    return *this * *this;
                }
                
                /**
                 Returns a normalized vector
                 
                 @return The normalized vector
                 */
                Vector<ValueType> Unitary() const {
                    return (* this) / Norm();
                }
                
                void Sort() {
                    std::sort(this->begin(), this->end(),
                              [](const ValueType &value1, const ValueType &value2) {
                                  return value1 < value2;
                              });
                }
                
                void Fill(const ValueType &value) {
                    std::fill(begin(), end(), value);
                }
                
                static Vector<ValueType> Zero(const size_t &size) {
                    return Vector<ValueType>(size, 0);
                }
                
                void Zero() {
                    Fill(0);
                }
                
                static Vector<ValueType> Ones(const size_t &size) {
                    return Vector<ValueType>(size, 1);
                }
                
                void Ones() {
                    Fill(1);
                }
                
                static Vector<ValueType> Random(const size_t &size, const ValueType &min, const ValueType &max) {
                    Vector<ValueType> random(size);
                    random.Random(min, max);
                    return random;
                }
                
                void Random(const ValueType &min = 0, const ValueType &max = 1) {
                    for (auto it = begin(); it != end(); ++it) {
                        *it = static_cast<ValueType>(drand48() * max + min);
                    }
                }
                
                void Clear() {
                    n = 0;
                    AllocateMemory(n);
                }
                
                bool IsEmpty() const {
                    return n == 0;
                }
                
                bool IsNull() const {
                    for (auto it = begin(); it != end(); ++it) {
                        if (*it != 0) {
                            return false;
                        }
                    }
                    return true;
                }
                
                bool HasDuplicate(const ValueType &accuracy) const {
                    ValueType distance;
                    for (auto it1 = begin(); it1 != end(); ++it1) {
                        for (auto it2 = begin(); it2 != end(); ++it2) {
                            if (it1 != it2) {
                                distance = *it1 - *it2;
                                if (std::sqrt(distance * distance) < accuracy) {
                                    return true;
                                }
                            }
                        }
                    }
                    return false;
                }
                
                bool HasDuplicate() const {
                    for (auto it1 = begin(); it1 != end(); ++it1) {
                        for (auto it2 = begin(); it2 != end(); ++it2) {
                            if (it1 != it2 && *it1 == *it2) {
                                return true;
                            }
                        }
                    }
                    return false;
                }
                
                ValueType * Find(const ValueType &value) const {
                    return cda::math::algorithms::find::Element(begin(), end(), value);
                }
                
                //  --- OPERATORS ---
                Vector<ValueType> operator+(const Vector<ValueType> &vector) const {
                    if (this->n != vector.n) {
                        throw std::logic_error("Unable to sum two vector of different size");
                    }
                    
                    Vector<ValueType> new_vector(this->n);
                    
                    auto it_new = new_vector.begin();
                    auto it_vector = vector.begin();
                    for (auto it_this = this->begin(); it_this != this->end(); ++it_this, ++it_vector, ++it_new) {
                        *it_new = *it_this + *it_vector;
                    }
                    
                    return new_vector;
                }
                
                Vector<ValueType> &operator+=(const Vector<ValueType> &vector) {
                    if (this->n != vector.n) {
                        throw std::logic_error("Unable to sum two vector of different size");
                    }
                    
                    auto it_vector = vector.begin();
                    for (auto it_this = this->begin(); it_this != this->end(); ++it_this, ++it_vector) {
                        *it_this += *it_vector;
                    }
                    
                    return *this;
                }
                
                Vector<ValueType> operator-(const Vector<ValueType> &vector) const {
                    if (this->n != vector.n) {
                        throw std::logic_error("Unable to sum two vector of different sizes");
                    }
                    
                    Vector<ValueType> new_vector(this->n);
                    
                    auto it_new = new_vector.begin();
                    auto it_vector = vector.begin();
                    for (auto it_this = this->begin(); it_this != this->end(); ++it_this, ++it_vector, ++it_new) {
                        *it_new = *it_this - *it_vector;
                    }
                    
                    return new_vector;
                }
                
                Vector<ValueType> &operator-=(const Vector<ValueType> &vector) {
                    if (this->n != vector.n) {
                        throw std::logic_error("Unable to sum two vector of different size");
                    }
                    
                    auto it_vector = vector.begin();
                    for (auto it_this = this->begin(); it_this != this->end(); ++it_this, ++it_vector) {
                        *it_this -= *it_vector;
                    }
                    
                    return *this;
                }
                
                Vector<ValueType> operator*(const ValueType &value) const {
                    Vector<ValueType> new_vector(this->n);
                    auto it_new = new_vector.begin();
                    
                    for (auto it_this = this->begin(); it_this != this->end(); ++it_this, ++it_new) {
                        *it_new = *it_this * value;
                    }
                    
                    return new_vector;
                }
                
                Vector<ValueType> &operator*=(const ValueType &value) {
                    for (auto it = this->begin(); it != this->end(); ++it) {
                        *it *= value;
                    }
                    return *this;
                }
                
                Vector<ValueType> operator/(const ValueType &value) const {
                    Vector<ValueType> new_vector(this->n);
                    auto it_new = new_vector.begin();
                    
                    for (auto it = this->begin(); it != this->end(); ++it, ++it_new) {
                        *it_new = *it / value;
                    }
                    
                    return new_vector;
                }
                
                Vector<ValueType> &operator/=(const ValueType &value) {
                    for (auto it = this->begin(); it != this->end(); ++it) {
                        *it /= value;
                    }
                    return *this;
                }
                
                Vector<ValueType> PowElements(const size_t &power) const {
                    Vector<ValueType> new_vector;
                    
                    switch (power) {
                        case 0:
                            new_vector = Vector<ValueType>::Ones(this->n);
                            break;
                        
                        default:
                            new_vector = *this;
                            ValueType *it_new_vector;
                            for (size_t i = 2; i <= power; ++i) {
                                it_new_vector = new_vector.begin();
                                for (auto it_this = this->begin(); it_this != this->end(); ++it_this) {
                                    *it_new_vector++ *= *it_this;
                                }
                            }
                            break;
                    }
                    
                    return new_vector;
                }
                
                Vector<ValueType> Sqrt() const {
                    Vector<ValueType> new_vector(this->n);
                    auto it_new_vector = new_vector.begin();
                    for (auto it_this = this->begin(); it_this != this->end(); ++it_this) {
                        *it_new_vector++ = std::sqrt(*it_this);
                    }
                    return new_vector;
                }
                
                template<typename Integer,
                         typename = std::enable_if<std::is_integral<Integer>::value>>
                Vector<ValueType> operator%(const Integer &value) const {
                    Vector<ValueType> new_vector(this->n);
                    auto it_new = new_vector.begin();
                    
                    for (auto it = this->begin(); it != this->end(); ++it, ++it_new) {
                        *it_new = static_cast<Integer>(*it) % value;
                    }
                    
                    return new_vector;
                }
                
                template<typename Integer,
                         typename = std::enable_if<std::is_integral<Integer>::value>>
                Vector<ValueType> &operator%=(const Integer& value) {
                    for (auto it = this->begin(); it != this->end(); ++it) {
                        *it = static_cast<Integer>(*it) % value;
                    }
                    return *this;
                }
                
                Vector<ValueType> operator-() const {
                    Vector<ValueType> new_vector(this->n);
                    auto it_new = new_vector.begin();
                    
                    for (auto it = this->begin(); it != this->end(); ++it) {
                        *it_new++ = -(*it);
                    }
                    
                    return new_vector;
                }
                
                Vector<ValueType> CrossProduct3D(const Vector<ValueType> &vector) const {
                    if (this->n != vector.n || this->n != 3) {
                        throw std::logic_error("Both vectors must be of the same size, and size must be 3");
                    }
                    
                    Vector<ValueType> new_vector(this->n);
                    new_vector[0] = this->operator[](1) * vector[2] - this->operator[](2) * vector[1];
                    new_vector[1] = this->operator[](2) * vector[0] - this->operator[](0) * vector[2];
                    new_vector[2] = this->operator[](0) * vector[1] - this->operator[](1) * vector[0];
                    
                    return new_vector;
                }
                
                ValueType operator*(const Vector<ValueType> &vector) const {
                    if (this->n != vector.n) {
                        throw std::logic_error("Both vectors must be of the same size");
                    }
                    
                    ValueType value(0);
                    auto it_vector = vector.begin();
                    
                    for (auto it_this = this->begin(); it_this != this->end(); ++it_this, ++it_vector) {
                        value += *it_this * *it_vector;
                    }
                    
                    return value;
                }
                
            };
    
        } /* namespace math */
    } /* namespace containers */
} /* namespace containers */

//  --- MORE OPERATORS ---
template <typename ValueType, typename T2>
cda::math::containers::Vector<ValueType> operator*(const T2 &value, const cda::math::containers::Vector<ValueType> &vector) {
    
    cda::math::containers::Vector<ValueType> new_vector(vector.Size());
    auto it_new = new_vector.begin();
    
    for (auto it = vector.begin(); it != vector.end(); ++it, ++it_new) {
        *it_new = *it * static_cast<ValueType>(value);
    }
    
    return new_vector;
}

template <typename ValueType>
void operator>>(std::istream &input,
                cda::math::containers::Vector<ValueType> &vector) {
    
    if (!input) {
        throw std::logic_error("Input is not avaiable");
    }
    
    size_t initial_size = 100;
    vector.resize(initial_size);
    
    char separator;
    std::string line, cell;
    size_t element = 0;
    if (std::getline(input, line)) {
        std::stringstream line_stream(line);
        while (line_stream >> vector[element]) {
            ++element;
            
            if (element >= vector.Size()) {
                initial_size *= 2;
                vector.resize(vector.Size() + initial_size);
            }
            
            line_stream >> separator;
        }
    }
    
    vector.resize(element);
}

template <typename ValueType>
std::ostream& operator<<(std::ostream &output,
                         const cda::math::containers::Vector<ValueType> &vector) {
    
    if (output.rdbuf() == std::cout.rdbuf()) {
        
        const size_t custom_width = 12;
        const size_t custom_precision = 5;
        
        output.width();
        output << std::fixed;
        output.fill(' ');
        output.precision(custom_precision);
        
        output << "[";
        output.width(custom_width);
        
        auto it_vector = vector.begin();
        output << *it_vector;
        for (it_vector = std::next(it_vector); it_vector < vector.end(); ++it_vector) {
            output << std::right << " ";
            output.width(custom_width);
            output << *it_vector;
        }
        output.width();
        output << "]" << std::endl;
        
        output.precision();
        
    } else {
        const char separator = ';';
        auto it_vector = vector.begin();
        output << *it_vector;
        for (it_vector = std::next(it_vector); it_vector < vector.end(); ++it_vector) {
            output << std::right << separator;
            output << *it_vector;
        }
        output << std::endl;
    }
    
    return output;
}
