#include <iostream>
#include <string>
using namespace std;

class BigInt
{
    string number;   // Stores the number as a string
    bool isNegative; // True if number is negative

    // Remove unnecessary leading zeros from the number string
    void removeLeadingZeros()
    {
        size_t nonZeroPos = 0;
        while (nonZeroPos < number.size() - 1 && number[nonZeroPos] == '0') {
            ++nonZeroPos;
        }
        number = number.substr(nonZeroPos);
    }

    // Compare absolute values of two BigInts (ignore signs)
    // Returns: 1 if |this| > |other|, 0 if equal, -1 if |this| < |other|
    int compareMagnitude(const BigInt &other) const
    {
        if (number.size() > other.number.size()) return 1;
        if (number.size() < other.number.size()) return -1;

        for (size_t i = 0; i < number.size(); ++i) {
            if (number[i] > other.number[i]) return 1;
            if (number[i] < other.number[i]) return -1;
        }
        return 0;
    }

public:
    // Default constructor - initialize to zero
    BigInt()
    {
        number = "0";
        isNegative = false;
    }

    // Constructor from 64-bit integer
    BigInt(int64_t value)
    {
        if(value < 0){
            isNegative = true;
        }
        else{
            isNegative = false;
        }

        number = std::to_string((value < 0 ? -value : value));
    }

    // Constructor from string representation
    BigInt(const string &str)
    {
        if(!str.empty() && str[0] == '-'){
            isNegative = true;
            number = str.substr(1, str.size());
        }
        else{
            isNegative = false;
            number = str;
        }

        this->removeLeadingZeros();
    }

    // Copy constructor
    BigInt(const BigInt &other)
    {
        this->number = other.number;
        this->isNegative = other.isNegative;
    }

    // Destructor
    ~BigInt()
    {

    }

    // Assignment operator
    BigInt &operator=(const BigInt &other)
    {
        if (this != &other) {
            number = other.number;
            isNegative = other.isNegative;
        }
        return *this;
    }

    // Unary negation operator (-x)
    BigInt operator-() const
    {
        lhs -= rhs;
        return lhs;
    }

    // Unary plus operator (+x)
    BigInt operator+() const
    {
        lhs += rhs;
        return lhs;
    }

    // Addition assignment operator (x += y)
    BigInt &operator+=(const BigInt &other)
    {
        if (other.number == "0") return *this;

        if (number == "0") {
            number = other.number;
            isNegative = other.isNegative;
            return *this;
        }
        if (isNegative == other.isNegative) {
            string result = "";
            int carry = 0;
            int i = number.size() - 1;
            int j = other.number.size() - 1;

            while (i >= 0 || j >= 0 || carry) {
                int sum = carry;
                if (i >= 0) sum += number[i--] - '0';
                if (j >= 0) sum += other.number[j--] - '0';
                carry = sum / 10;
                sum %= 10;
                result = char(sum + '0') + result;
            }
            number = result;
            } else {
                BigInt temp = other;
                temp.isNegative = !temp.isNegative;
                *this -= temp;
            }

            return *this;

    }

    // Subtraction assignment operator (x -= y)
    BigInt &operator-=(const BigInt &other)
    {
        if (other.number == "0") return *this;

        if (number == "0") {
            number = other.number;
            isNegative = !other.isNegative;
            return *this;
        }
        if (isNegative != other.isNegative) {
            BigInt temp = other;
            temp.isNegative = !temp.isNegative;
            *this += temp;
        } else {
            int cmp = compareMagnitude(other);
            if (cmp == 0) {
                number = "0";
                isNegative = false;
            } else {
                string larger, smaller;
                bool resultNegative;

                if (cmp > 0) {
                    larger = number;
                    smaller = other.number;
                    resultNegative = isNegative;
                } else {
                    larger = other.number;
                    smaller = number;
                    resultNegative = !isNegative;
                }

                string result = "";
                int borrow = 0;
                int i = larger.size() - 1;
                int j = smaller.size() - 1;

                while (i >= 0) {
                    int diff = (larger[i] - '0') - borrow;
                    if (j >= 0) diff -= (smaller[j] - '0');
                    if (diff < 0) {
                    diff += 10;
                        borrow = 1;
                    } else {
                        borrow = 0;
                    }
                    result = char(diff + '0') + result;
                    i--; j--;
                }

                number = result;
                removeLeadingZeros();
                isNegative = resultNegative;
            }
        }

        return *this;
    }

    // Multiplication assignment operator (x *= y)
    BigInt &operator*=(const BigInt &other)
    {
        // TODO: Implement this operator
        return *this;
    }

    // Division assignment operator (x /= y)
    BigInt &operator/=(const BigInt &other)
    {
        // TODO: Implement this operator
        return *this;
    }

    // Modulus assignment operator (x %= y)
    BigInt &operator%=(const BigInt &other)
    {
        // TODO: Implement this operator
        return *this;
    }

    // Pre-increment operator (++x)
    BigInt &operator++()
    {
        *this += BigInt(1);
        return *this;
    }

    // Post-increment operator (x++)
    BigInt operator++(int)
    {
        BigInt temp = *this;
        ++(*this);
        return temp;
    }

    // Pre-decrement operator (--x)
    BigInt &operator--()
    {
        *this -= BigInt(1);
        return *this;
    }

    // Post-decrement operator (x--)
    BigInt operator--(int)
    {
        BigInt temp = *this;
        --(*this);
        return temp;
    }

    // Convert BigInt to string representation
    string toString() const
    {
        return (isNegative && number != "0" ? "-" : "") + number;
    }

    friend ostream &operator<<(ostream &os, const BigInt &num)
    {
        os << num.toString();
        return os;
    }

    // Output stream operator (for printing)
    friend ostream &operator<<(ostream &os, const BigInt &num)
    {
        // TODO: Implement this operator
        return os;
    }

    // Input stream operator (for reading from input)
    friend istream &operator>>(istream &is, BigInt &num)
    {
        // TODO: Implement this operator
        return is;
    }

    // Friend declarations for comparison operators
    friend bool operator==(const BigInt &lhs, const BigInt &rhs);
    friend bool operator<(const BigInt &lhs, const BigInt &rhs);
};

// Binary addition operator (x + y)
BigInt operator+(BigInt lhs, const BigInt &rhs)
{
    BigInt result;
    // TODO: Implement this operator
    return result;
}

// Binary subtraction operator (x - y)
BigInt operator-(BigInt lhs, const BigInt &rhs)
{
    BigInt result;
    // TODO: Implement this operator
    return result;
}

// Binary multiplication operator (x * y)
BigInt operator*(BigInt lhs, const BigInt &rhs)
{
    BigInt result;
    // TODO: Implement this operator
    return result;
}

// Binary division operator (x / y)
BigInt operator/(BigInt lhs, const BigInt &rhs)
{
    BigInt result;
    // TODO: Implement this operator
    return result;
}

// Binary modulus operator (x % y)
BigInt operator%(BigInt lhs, const BigInt &rhs)
{
    BigInt result;
    // TODO: Implement this operator
    return result;
}

// Equality comparison operator (x == y)
bool operator==(const BigInt &lhs, const BigInt &rhs)
{
    return lhs.isNegative == rhs.isNegative && lhs.number == rhs.number;
}

// Inequality comparison operator (x != y)
bool operator!=(const BigInt &lhs, const BigInt &rhs)
{
    return !(lhs == rhs);
}

// Less-than comparison operator (x < y)
bool operator<(const BigInt &lhs, const BigInt &rhs)
{
    if(lhs.isNegative != rhs.isNegative) return lhs.isNegative;
    int cmp = lhs.compareMagnitude(rhs);
    return lhs.isNegative ? cmp > 0 : cmp < 0;
}

// Less-than-or-equal comparison operator (x <= y)
bool operator<=(const BigInt &lhs, const BigInt &rhs)
{
    return (lhs < rhs) || (lhs == rhs);
}

// Greater-than comparison operator (x > y)
bool operator>(const BigInt &lhs, const BigInt &rhs)
{
    return !(lhs <= rhs);
}

// Greater-than-or-equal comparison operator (x >= y)
bool operator>=(const BigInt &lhs, const BigInt &rhs)
{
    return !(lhs < rhs);
}

int main()
{
    cout << "=== BigInt Class Test Program ===" << endl
         << endl;
    cout << "NOTE: All functions are currently empty." << endl;
    cout << "Your task is to implement ALL the functions above." << endl;
    cout << "The tests below will work once you implement them correctly." << endl
         << endl;

    /*
    // Test 1: Constructors and basic output
    cout << "1. Constructors and output:" << endl;
    BigInt a(12345);              // Should create BigInt from integer
    BigInt b("-67890");           // Should create BigInt from string
    BigInt c("0");                // Should handle zero correctly
    BigInt d = a;                 // Should use copy constructor
    cout << "a (from int): " << a << endl;        // Should print "12345"
    cout << "b (from string): " << b << endl;     // Should print "-67890"
    cout << "c (zero): " << c << endl;            // Should print "0"
    cout << "d (copy of a): " << d << endl << endl; // Should print "12345"

    // Test 2: Arithmetic operations
    cout << "2. Arithmetic operations:" << endl;
    cout << "a + b = " << a + b << endl;          // Should calculate 12345 + (-67890)
    cout << "a - b = " << a - b << endl;          // Should calculate 12345 - (-67890)
    cout << "a * b = " << a * b << endl;          // Should calculate 12345 * (-67890)
    cout << "b / a = " << b / a << endl;          // Should calculate (-67890) / 12345
    cout << "a % 100 = " << a % BigInt(100) << endl << endl; // Should calculate 12345 % 100

    // Test 3: Relational operators
    cout << "3. Relational operators:" << endl;
    cout << "a == d: " << (a == d) << endl;       // Should be true (12345 == 12345)
    cout << "a != b: " << (a != b) << endl;       // Should be true (12345 != -67890)
    cout << "a < b: " << (a < b) << endl;         // Should be false (12345 < -67890)
    cout << "a > b: " << (a > b) << endl;         // Should be true (12345 > -67890)
    cout << "c == 0: " << (c == BigInt(0)) << endl << endl; // Should be true (0 == 0)

    // Test 4: Unary operators and increments
    cout << "4. Unary operators and increments:" << endl;
    cout << "-a: " << -a << endl;                 // Should print "-12345"
    cout << "++a: " << ++a << endl;               // Should increment and print "12346"
    cout << "a--: " << a-- << endl;               // Should print "12346" then decrement
    cout << "a after decrement: " << a << endl << endl; // Should print "12345"

    // Test 5: Large number operations
    cout << "5. Large number operations:" << endl;
    BigInt num1("12345678901234567890");
    BigInt num2("98765432109876543210");
    cout << "Very large addition: " << num1 + num2 << endl;
    cout << "Very large multiplication: " << num1 * num2 << endl << endl;

    // Test 6: Edge cases and error handling
    cout << "6. Edge cases:" << endl;
    BigInt zero(0);
    BigInt one(1);
    try {
        BigInt result = one / zero;               // Should throw division by zero error
        cout << "Division by zero succeeded (unexpected)" << endl;
    } catch (const runtime_error& e) {
        cout << "Division by zero correctly threw error: " << e.what() << endl;
    }
    cout << "Multiplication by zero: " << one * zero << endl;        // Should be "0"
    cout << "Negative multiplication: " << BigInt(-5) * BigInt(3) << endl;  // Should be "-15"
    cout << "Negative division: " << BigInt(-10) / BigInt(3) << endl;       // Should be "-3"
    cout << "Negative modulus: " << BigInt(-10) % BigInt(3) << endl;        // Should be "-1"
    */

    return 0;
}
