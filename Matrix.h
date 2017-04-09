/* 
 * File:   Matrix.h
 * Author: Sergey
 *
 * Created on April 8, 2017, 9:05 AM
 */

#ifndef MATRIX_H
#define	MATRIX_H

#include <vector> 
#include <string>
#include <map>
#include <algorithm>
#include <functional>
#include <cassert>

using namespace std;

class Matrix {
public:
    enum ConstaintType {
        equal,
        notEqual,
        greater,
        greaterOrEqual,
        less,
        lessOrEqual
    };
    
    struct Constraint {
        static map<ConstaintType, function<bool(int,int)>> validationMap;
        
        ConstaintType type;
        int value;
        
        bool check(int val) const {
            auto iter = validationMap.find(type);
            assert(iter != validationMap.end());
            return (*iter).second(val, value);
        }
    };
    
    enum ExceptionType {
        constraintViolation,
        invalidSize,
        outOfRange
    };
    
    class Exception : public std::exception {
    private:
        ExceptionType m_type;
        
    public:
        Exception() = delete;
        explicit Exception(ExceptionType type) : std::exception(), m_type(type) {}
        Exception(const Exception&) = default;
        ~Exception() = default;
        
        virtual const char* what() const noexcept {
            return Matrix::errMsg[m_type].c_str();
        }
    };
    
protected:
    static vector<string> errMsg;
    
    size_t m_numRows;
    size_t m_numColumns;
    vector<vector<int>> m_Data;
    vector<Constraint> m_Constraints;
    
public:
    // ctor/dtor
    Matrix() = default;
    
    Matrix(size_t numRows, size_t numColumns, int fillWith = 0, const vector<Constraint>& constraints = {}) throw(Exception) :
        m_numRows(numRows),
        m_numColumns(numColumns),
        m_Constraints(constraints)
    {
        checkConstraints(fillWith);
        
        for (size_t i = 0; i < m_numRows; ++i) {
            auto row = vector<int>(m_numColumns);
            fill(row.begin(), row.end(), fillWith);
            m_Data.push_back(row);
        }
    }
    
    Matrix(const vector<vector<int>>& data, int fillWith = 0, const vector<Constraint>& constraints = {}) :
        m_numRows(data.size()),
        m_numColumns(0),
        m_Data(data),
        m_Constraints(constraints)
    {
        if (m_Data.size() > 0) {
            m_numColumns = m_Data.front().size();
            auto first = m_Data.begin();

            for_each(m_Data.begin(), m_Data.end(), [&](const vector<int>& row) {
               if (row.size() != m_numColumns) {
                   throw Exception(invalidSize);
               }
            });
        }
    }
    
    Matrix(const Matrix&) = default;
    Matrix(Matrix&&) = default;
    
    Matrix& operator = (const Matrix&) = default;
    
    ~Matrix() = default;
    
    // Data access
    const vector<int>& getRow(size_t row) const throw (Exception) {
        // check index
        if (row > m_numRows) {
            throw Exception(outOfRange);
        }
        return m_Data[row];
    }
    
    void setRow(size_t row, const vector<int>& data) throw (Exception) {
        if (row > m_numRows) {
            throw Exception(outOfRange);
        }
        if (data.size() != m_numColumns) {
            throw Exception(invalidSize);
        }
        checkConstraints(data);
        m_Data[row] = data;
    }
    
    void setRow(size_t row, vector<int>&& data) throw (Exception) {
        if (row > m_numRows) {
            throw Exception(outOfRange);
        }
        if (data.size() != m_numColumns) {
            throw Exception(invalidSize);
        }
        checkConstraints(data);
        m_Data[row] = data;
    }
    
    vector<int>&& getColumn(size_t col) const throw (Exception) {
        if (col > m_numRows) {
            throw Exception(outOfRange);
        }
        vector<int> colData;
        for_each(m_Data.begin(), m_Data.end(), [&](const vector<int>& row) {colData.push_back(row[col]);});
        return std::move(colData);
    }
    
    void setColumn(size_t col, const vector<int>& data) throw (Exception) {
        if (col > m_numColumns) {
            throw Exception(outOfRange);
        }
        if (data.size() != m_numRows) {
            throw Exception(invalidSize);
        }
        checkConstraints(data);
        
        size_t row = 0;
        for_each(data.begin(), data.end(), [&](int val) {
            m_Data[row++][col] = val;
        });
    }

    void setColumn(size_t col, vector<int>&& data) throw (Exception) {
        if (col > m_numColumns) {
            throw Exception(outOfRange);
        }
        if (data.size() != m_numRows) {
            throw Exception(invalidSize);
        }
        checkConstraints(data);
        
        size_t row = 0;
        for_each(data.begin(), data.end(), [&](int val) {
            m_Data[row++][col] = val;
        });
    }

protected:
    void checkConstraints(int value) const throw (Exception) {
        for_each(m_Constraints.begin(), m_Constraints.end(), [=](const Matrix::Constraint c) {
            if (!c.check(value)) {
                throw Exception(constraintViolation);
            }
        });
    }
    
    void checkConstraints() const throw (Exception) {
        for_each(m_Data.begin(), m_Data.end(), [&](const vector<int>& row) {
           for_each(row.begin(), row.end(), [&](const int& val) {
               checkConstraints(val);
           }); 
        });
    }
    
    void checkConstraints(const vector<int>& values) const throw (Exception) {
        for_each(values.begin(), values.end(), [&](const int& val) {
            checkConstraints(val);
        });
    }
    
};

vector<string> Matrix::errMsg = {
    "One or more elements of matrix violate constraints.",
    "All rows in matrix must have the same size.",
    "Index is out of range when requested raw/column data."
};

map<Matrix::ConstaintType, function<bool(int,int)>> Matrix::Constraint::validationMap = {
    {Matrix::equal, [](int valToCheck, int value) {return valToCheck == value;}},
    {Matrix::notEqual, [](int valToCheck, int value) {return valToCheck != value;}},
    {Matrix::greater, [](int valToCheck, int value) {return valToCheck > value;}},
    {Matrix::greaterOrEqual, [](int valToCheck, int value) {return valToCheck >= value;}},
    {Matrix::less, [](int valToCheck, int value) {return valToCheck < value;}},
    {Matrix::lessOrEqual, [](int valToCheck, int value) {return valToCheck <= value;}}
};

#endif	/* MATRIX_H */

