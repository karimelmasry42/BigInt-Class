#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <stdexcept>

#define cti(X) ((X) - 48)
#define itc(X) ((X) + 48)

using namespace std;

// Aggregated result from a single test suite.
struct TestResult { int passed = 0, failed = 0; };

// Forward declarations (BIGINT-26: separate test blocks)
static TestResult runNormalizationTests();
static TestResult runNormalizationEdgeCaseTests();
static TestResult runConstructorTests();
static TestResult runAssignmentAndOutputTests();
static TestResult runInputStreamTests();
static TestResult runAdditionTests();
static TestResult runSubtractionTests();
static TestResult runMultiplicationTests();
static TestResult runDivisionTests();
static TestResult runModulusTests();

class BigInt
{
    string number;   // Stores the number as a string (digits only, no sign)
    bool isNegative; // True if number is negative

    // Remove unnecessary leading zeros from the number string
    void removeLeadingZeros()
    {
        size_t start = number.find_first_not_of('0');
        if (start == string::npos)
            number = "0";
        else
            number = number.substr(start);

        if (number == "0")
            isNegative = false;
    }

    // Compare absolute values of two BigInts (ignore signs)
    // Returns: 1 if |this| > |other|, 0 if equal, -1 if |this| < |other|
    int compareMagnitude(const BigInt &other) const
    {
        if (number.size() != other.number.size())
            return number.size() > other.number.size() ? 1 : -1;
        int cmp = number.compare(other.number);
        if (cmp > 0) return 1;
        if (cmp < 0) return -1;
        return 0;
    }

    // Add two magnitude strings digit-by-digit (right to left), return result magnitude string
    static string addMagnitudes(const string &a, const string &b)
    {
        string result;
        int i = (int)a.size() - 1, j = (int)b.size() - 1;
        int carry = 0;
        while (i >= 0 || j >= 0 || carry) {
            int sum = carry;
            if (i >= 0) sum += cti(a[i--]);
            if (j >= 0) sum += cti(b[j--]);
            result += itc(sum % 10);
            carry = sum / 10;
        }
        reverse(result.begin(), result.end());
        return result;
    }

    // Subtract magnitude b from magnitude a (|a| >= |b|), return result magnitude string
    static string subtractMagnitudes(const string &a, const string &b)
    {
        string result;
        int i = (int)a.size() - 1, j = (int)b.size() - 1;
        int borrow = 0;
        while (i >= 0) {
            int diff = cti(a[i--]) - borrow;
            if (j >= 0) diff -= cti(b[j--]);
            if (diff < 0) { diff += 10; borrow = 1; }
            else borrow = 0;
            result += itc(diff);
        }
        // Strip trailing zeros (they become leading zeros after reverse)
        while (result.size() > 1 && result.back() == '0') result.pop_back();
        reverse(result.begin(), result.end());
        return result;
    }

public:
    // Default constructor - initialize to zero
    BigInt() : number("0"), isNegative(false) {}

    // Constructor from 64-bit integer
    BigInt(int64_t value)
    {
        isNegative = value < 0;
        number = to_string(value < 0 ? -value : value);
    }

    // Constructor from string representation
    BigInt(const string &str)
    {
        if (str[0] == '-') {
            isNegative = true;
            number = str.substr(1);
        } else {
            isNegative = false;
            number = str;
        }
        removeLeadingZeros();
    }

    // Copy constructor
    BigInt(const BigInt &other) : number(other.number), isNegative(other.isNegative) {}

    // Destructor
    ~BigInt() {}

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
        BigInt result(*this);
        if (result.number != "0")
            result.isNegative = !result.isNegative;
        return result;
    }

    // Unary plus operator (+x)
    BigInt operator+() const
    {
        return *this;
    }

    // Addition assignment operator (x += y)
    // Covers: same-sign addition, mixed-sign (converted to magnitude subtraction),
    // sign assignment per SRS page-6 rules, normalization after operation.
    BigInt &operator+=(const BigInt &other)
    {
        int cmp = compareMagnitude(other);

        if (isNegative == other.isNegative) {
            // Same sign: add magnitudes, keep the shared sign
            number = addMagnitudes(number, other.number);
        } else {
            // Different signs: subtract smaller magnitude from larger
            if (cmp == 0) {
                // Equal magnitudes, opposite signs → zero
                number = "0";
                isNegative = false;
            } else if (cmp > 0) {
                // |this| > |other|: keep this sign
                number = subtractMagnitudes(number, other.number);
            } else {
                // |other| > |this|: take other's sign
                number = subtractMagnitudes(other.number, number);
                isNegative = other.isNegative;
            }
        }
        removeLeadingZeros();
        return *this;
    }

    // Subtraction assignment operator (x -= y)
    // Mixed-sign subtraction is converted to addition:  a - b == a + (-b)
    BigInt &operator-=(const BigInt &other)
    {
        BigInt negOther(other);
        if (negOther.number != "0")
            negOther.isNegative = !negOther.isNegative;
        return *this += negOther;
    }

    // Multiplication assignment operator (x *= y)
    // Schoolbook long multiplication; sign via XOR rule (page 7).
    BigInt &operator*=(const BigInt &other)
    {
        // Short-circuit if either operand is zero
        if (number == "0" || other.number == "0") {
            number = "0";
            isNegative = false;
            return *this;
        }

        const string &a = number;
        const string &b = other.number;
        int n = (int)a.size(), m = (int)b.size();
        vector<int> res(n + m, 0);

        // Multiply each digit pair and accumulate
        for (int i = n - 1; i >= 0; i--) {
            for (int j = m - 1; j >= 0; j--) {
                int prod = cti(a[i]) * cti(b[j]);
                int p1 = i + j, p2 = i + j + 1;
                int sum = prod + res[p2];
                res[p2] = sum % 10;
                res[p1] += sum / 10;
            }
        }

        // Build result string, trimming leading zeros
        string result;
        for (int d : res) {
            if (!(result.empty() && d == 0))
                result += itc(d);
        }
        number = result.empty() ? "0" : result;
        isNegative = isNegative != other.isNegative; // XOR sign rule
        removeLeadingZeros();
        return *this;
    }

    // Division assignment operator (x /= y)
    // Truncation toward zero; sign via XOR rule (page 7).
    BigInt &operator/=(const BigInt &other)
    {
        if (other.number == "0")
            throw runtime_error("Division by zero");

        int cmp = compareMagnitude(other);
        if (cmp < 0) {
            // |this| < |other|: truncation toward zero gives 0
            number = "0";
            isNegative = false;
            return *this;
        }

        bool resultNeg = isNegative != other.isNegative; // XOR sign rule

        // Work on absolute values using digit-by-digit long division
        const string &dividendStr = number;
        const string &divisorStr  = other.number;
        BigInt divisor(divisorStr);

        string quotient;
        string current; // running partial dividend

        for (char digit : dividendStr) {
            current += digit;
            // Strip leading zeros from partial dividend
            size_t s = current.find_first_not_of('0');
            current = (s != string::npos) ? current.substr(s) : "0";

            // Count how many times divisor fits into current (at most 9)
            int count = 0;
            BigInt cur(current);
            while (cur.compareMagnitude(divisor) >= 0) {
                cur.number = subtractMagnitudes(cur.number, divisor.number);
                cur.removeLeadingZeros();
                ++count;
            }
            quotient += itc(count);
            current = cur.number; // remainder becomes next partial dividend
        }

        // Strip leading zeros from quotient
        size_t s = quotient.find_first_not_of('0');
        number = (s != string::npos) ? quotient.substr(s) : "0";
        isNegative = resultNeg;
        removeLeadingZeros();
        return *this;
    }

    // Modulus assignment operator (x %= y)
    // Result sign matches the dividend (this), per page 7 and page 12 warning.
    BigInt &operator%=(const BigInt &other)
    {
        if (other.number == "0")
            throw runtime_error("Division by zero");

        bool dividendNeg = isNegative;
        // remainder = dividend - (dividend / divisor) * divisor
        BigInt q(*this);
        q /= other;
        q *= other;
        *this -= q;
        // Enforce: result sign matches original dividend sign
        if (number == "0") isNegative = false;
        else               isNegative = dividendNeg;
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
        BigInt temp(*this);
        *this += BigInt(1);
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
        BigInt temp(*this);
        *this -= BigInt(1);
        return temp;
    }

    // Convert BigInt to string representation
    string toString() const
    {
        if (isNegative && number != "0")
            return "-" + number;
        return number;
    }

    // Output stream operator (for printing)
    friend ostream &operator<<(ostream &os, const BigInt &num)
    {
        os << num.toString();
        return os;
    }

    // Input stream operator (for reading from input)
    friend istream &operator>>(istream &is, BigInt &num)
    {
        string token;
        is >> token;
        num = BigInt(token);
        return is;
    }

    // Friend declarations for comparison operators
    friend bool operator==(const BigInt &lhs, const BigInt &rhs);
    friend bool operator<(const BigInt &lhs, const BigInt &rhs);

    // BIGINT-26: friend access for each test suite
    friend TestResult runNormalizationTests();
    friend TestResult runNormalizationEdgeCaseTests();
    friend TestResult runConstructorTests();
    friend TestResult runAssignmentAndOutputTests();
    friend TestResult runInputStreamTests();
    friend TestResult runAdditionTests();
    friend TestResult runSubtractionTests();
    friend TestResult runMultiplicationTests();
    friend TestResult runDivisionTests();
    friend TestResult runModulusTests();
    friend TestResult runUnaryAndIncDecTests();
    friend TestResult runComparisonTests();
    friend TestResult runEdgeCaseAuditTests();
};

// ---------------------------------------------------------------------------
// BIGINT-42: Binary arithmetic operators — thin wrappers over compound operators
// ---------------------------------------------------------------------------

BigInt operator+(BigInt lhs, const BigInt &rhs) { lhs += rhs; return lhs; }
BigInt operator-(BigInt lhs, const BigInt &rhs) { lhs -= rhs; return lhs; }
BigInt operator*(BigInt lhs, const BigInt &rhs) { lhs *= rhs; return lhs; }
BigInt operator/(BigInt lhs, const BigInt &rhs) { lhs /= rhs; return lhs; }
BigInt operator%(BigInt lhs, const BigInt &rhs) { lhs %= rhs; return lhs; }

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
    // Different signs: negative < positive
    if (lhs.isNegative != rhs.isNegative)
        return lhs.isNegative;

    // Same sign: compare magnitudes
    int cmp = lhs.compareMagnitude(rhs);
    // Both positive: smaller magnitude means smaller value
    // Both negative: smaller magnitude means larger value (more to the right on number line)
    return lhs.isNegative ? (cmp > 0) : (cmp < 0);
}

// Less-than-or-equal comparison operator (x <= y)
bool operator<=(const BigInt &lhs, const BigInt &rhs)
{
    return !(rhs < lhs);
}

// Greater-than comparison operator (x > y)
bool operator>(const BigInt &lhs, const BigInt &rhs)
{
    return rhs < lhs;
}

// Greater-than-or-equal comparison operator (x >= y)
bool operator>=(const BigInt &lhs, const BigInt &rhs)
{
    return !(lhs < rhs);
}

// ---------------------------------------------------------------------------
// BIGINT-9 / BIGINT-25: micro-tests for removeLeadingZeros and compareMagnitude
// BIGINT-29: checkEq shows expected vs actual on failure
// ---------------------------------------------------------------------------
static TestResult runNormalizationTests()
{
    TestResult r;
    auto check = [&](bool cond, const char* desc) {
        if (cond) { cout << "  PASS: " << desc << "\n"; ++r.passed; }
        else       { cout << "  FAIL: " << desc << "\n"; ++r.failed; }
    };
    auto checkEq = [&](const string& actual, const string& expected, const char* desc) {
        if (actual == expected) { cout << "  PASS: " << desc << "\n"; ++r.passed; }
        else {
            cout << "  FAIL: " << desc << "\n"
                 << "        expected: \"" << expected << "\"\n"
                 << "        actual:   \"" << actual   << "\"\n";
            ++r.failed;
        }
    };

    cout << "--- removeLeadingZeros ---\n";
    BigInt a("000123");  checkEq(a.number, "123", "\"000123\" -> \"123\"");
    BigInt b("0000");    checkEq(b.number, "0",   "\"0000\"  -> \"0\"");
    BigInt neg0("-0");   check(neg0.number == "0" && !neg0.isNegative, "\"-0\" normalized to non-negative zero");

    cout << "--- compareMagnitude ---\n";
    BigInt x("100"), y("99"), z("100"), w("9");
    check(x.compareMagnitude(y) == 1,  "longer > shorter (100 vs 99)");
    check(y.compareMagnitude(x) == -1, "shorter < longer (99 vs 100)");
    check(x.compareMagnitude(z) == 0,  "equal magnitudes (100 vs 100)");
    check(y.compareMagnitude(w) == 1,  "same length, lex greater (99 vs 9)");

    return r;
}

// ---------------------------------------------------------------------------
// BIGINT-28: normalization edge cases ("page 12 common mistakes")
// BIGINT-29: checkEq shows expected vs actual on failure
// ---------------------------------------------------------------------------
static TestResult runNormalizationEdgeCaseTests()
{
    TestResult r;
    auto checkEq = [&](const string& actual, const string& expected, const char* desc) {
        if (actual == expected) { cout << "  PASS: " << desc << "\n"; ++r.passed; }
        else {
            cout << "  FAIL: " << desc << "\n"
                 << "        expected: \"" << expected << "\"\n"
                 << "        actual:   \"" << actual   << "\"\n";
            ++r.failed;
        }
    };
    auto checkBool = [&](bool actual, bool expected, const char* desc) {
        if (actual == expected) { cout << "  PASS: " << desc << "\n"; ++r.passed; }
        else {
            cout << "  FAIL: " << desc << "\n"
                 << "        expected: " << (expected ? "true" : "false") << "\n"
                 << "        actual:   " << (actual   ? "true" : "false") << "\n";
            ++r.failed;
        }
    };

    cout << "--- positive leading zeros ---\n";
    { BigInt n("007");        checkEq(n.number, "7",  "\"007\" -> digits \"7\"");
                              checkBool(n.isNegative, false, "\"007\" -> not negative"); }
    { BigInt n("0001");       checkEq(n.number, "1",  "\"0001\" -> digits \"1\""); }
    { BigInt n("0000000000"); checkEq(n.number, "0",  "all-zero string -> digits \"0\"");
                              checkBool(n.isNegative, false, "all-zero -> not negative"); }
    { BigInt n("00000001");   checkEq(n.toString(), "1", "\"00000001\" toString -> \"1\""); }

    cout << "--- negative zero variants ---\n";
    { BigInt n("-0");    checkEq(n.number, "0",  "\"-0\"   -> digits \"0\"");
                         checkBool(n.isNegative, false, "\"-0\"   -> isNegative false"); }
    { BigInt n("-00");   checkEq(n.number, "0",  "\"-00\"  -> digits \"0\"");
                         checkBool(n.isNegative, false, "\"-00\"  -> isNegative false"); }
    { BigInt n("-000");  checkEq(n.number, "0",  "\"-000\" -> digits \"0\"");
                         checkBool(n.isNegative, false, "\"-000\" -> isNegative false"); }

    cout << "--- negative with leading zeros on non-zero value ---\n";
    { BigInt n("-007");  checkEq(n.number, "7",  "\"-007\" -> digits \"7\"");
                         checkBool(n.isNegative, true, "\"-007\" -> isNegative true");
                         checkEq(n.toString(), "-7", "\"-007\" toString -> \"-7\""); }
    { BigInt n("-0042"); checkEq(n.number, "42", "\"-0042\" -> digits \"42\"");
                         checkEq(n.toString(), "-42", "\"-0042\" toString -> \"-42\""); }

    cout << "--- zero consistency ---\n";
    { BigInt n("0");    checkEq(n.toString(), "0", "\"0\" toString -> \"0\"");
                        checkBool(n.isNegative, false, "\"0\" -> not negative"); }
    { BigInt n(0);      checkEq(n.toString(), "0", "BigInt(0) toString -> \"0\"");
                        checkBool(n.isNegative, false, "BigInt(0) -> not negative"); }
    { BigInt n(-0);     checkEq(n.toString(), "0", "BigInt(-0) toString -> \"0\"");
                        checkBool(n.isNegative, false, "BigInt(-0) -> not negative"); }

    return r;
}

// ---------------------------------------------------------------------------
// BIGINT-27: validate constructors, copy behavior, assignment, toString, <<
// BIGINT-29: checkEq shows expected vs actual on failure
// ---------------------------------------------------------------------------
static TestResult runConstructorTests()
{
    TestResult r;
    auto checkEq = [&](const string& actual, const string& expected, const char* desc) {
        if (actual == expected) { cout << "  PASS: " << desc << "\n"; ++r.passed; }
        else {
            cout << "  FAIL: " << desc << "\n"
                 << "        expected: \"" << expected << "\"\n"
                 << "        actual:   \"" << actual   << "\"\n";
            ++r.failed;
        }
    };
    auto checkBool = [&](bool actual, bool expected, const char* desc) {
        if (actual == expected) { cout << "  PASS: " << desc << "\n"; ++r.passed; }
        else {
            cout << "  FAIL: " << desc << "\n"
                 << "        expected: " << (expected ? "true" : "false") << "\n"
                 << "        actual:   " << (actual   ? "true" : "false") << "\n";
            ++r.failed;
        }
    };

    cout << "--- default constructor ---\n";
    { BigInt n;  checkEq(n.toString(), "0", "BigInt() -> \"0\"");
                 checkBool(n.isNegative, false, "BigInt() -> not negative"); }

    cout << "--- int64_t constructor ---\n";
    { BigInt n(42);         checkEq(n.toString(), "42",  "BigInt(42)"); }
    { BigInt n(-99);        checkEq(n.toString(), "-99", "BigInt(-99)"); }
    { BigInt n(0);          checkEq(n.toString(), "0",   "BigInt(0)");
                            checkBool(n.isNegative, false, "BigInt(0) not negative"); }
    { BigInt n(1000000000LL); checkEq(n.toString(), "1000000000", "BigInt(1000000000)"); }
    { BigInt n(-1000000000LL); checkEq(n.toString(), "-1000000000", "BigInt(-1000000000)"); }
    { BigInt n(INT64_MAX);  checkEq(n.number, "9223372036854775807", "BigInt(INT64_MAX) digits"); }

    cout << "--- string constructor ---\n";
    { BigInt n("12345");    checkEq(n.toString(), "12345",  "BigInt(\"12345\")"); }
    { BigInt n("-67890");   checkEq(n.toString(), "-67890", "BigInt(\"-67890\")"); }
    { BigInt n("0");        checkEq(n.toString(), "0",      "BigInt(\"0\")");
                            checkBool(n.isNegative, false, "BigInt(\"0\") not negative"); }
    { BigInt n("123456789012345678901234567890");
      checkEq(n.toString(), "123456789012345678901234567890", "BigInt(large positive string)"); }
    { BigInt n("-987654321098765432109876543210");
      checkEq(n.toString(), "-987654321098765432109876543210", "BigInt(large negative string)"); }

    cout << "--- copy constructor ---\n";
    {
        BigInt src(12345);
        BigInt dst(src);
        checkEq(dst.toString(), "12345", "copy of positive: same value");
        checkBool(dst.isNegative, false, "copy of positive: not negative");
        src = BigInt(99);
        checkEq(dst.toString(), "12345", "copy is independent after src reassignment");
    }
    {
        BigInt src("-999");
        BigInt dst(src);
        checkEq(dst.toString(), "-999", "copy of negative: same value");
        checkBool(dst.isNegative, true,  "copy of negative: isNegative true");
    }
    {
        BigInt src(0);
        BigInt dst(src);
        checkEq(dst.toString(), "0",   "copy of zero: \"0\"");
        checkBool(dst.isNegative, false, "copy of zero: not negative");
    }

    return r;
}

// ---------------------------------------------------------------------------
// BIGINT-21 / BIGINT-27: unit tests for assignment operator and stream output
// BIGINT-29: checkEq shows expected vs actual on failure
// ---------------------------------------------------------------------------
static TestResult runAssignmentAndOutputTests()
{
    TestResult r;
    auto checkEq = [&](const string& actual, const string& expected, const char* desc) {
        if (actual == expected) { cout << "  PASS: " << desc << "\n"; ++r.passed; }
        else {
            cout << "  FAIL: " << desc << "\n"
                 << "        expected: \"" << expected << "\"\n"
                 << "        actual:   \"" << actual   << "\"\n";
            ++r.failed;
        }
    };
    auto checkBool = [&](bool actual, bool expected, const char* desc) {
        if (actual == expected) { cout << "  PASS: " << desc << "\n"; ++r.passed; }
        else {
            cout << "  FAIL: " << desc << "\n"
                 << "        expected: " << (expected ? "true" : "false") << "\n"
                 << "        actual:   " << (actual   ? "true" : "false") << "\n";
            ++r.failed;
        }
    };

    cout << "--- assignment operator ---\n";

    BigInt src(42);
    BigInt dst;
    dst = src;
    checkEq(dst.number, "42",  "copy assignment: value copied");
    checkBool(dst.isNegative, false, "copy assignment: sign copied");

    BigInt self("999");
    self = self;
    checkEq(self.number, "999", "self-assignment: value unchanged");
    checkBool(self.isNegative, false, "self-assignment: sign unchanged");

    BigInt a, b;
    a = b = BigInt("-7");
    checkEq(a.number, "7",  "chained assignment: a gets -7 (digits)");
    checkBool(a.isNegative, true, "chained assignment: a is negative");
    checkEq(b.number, "7",  "chained assignment: b gets -7 (digits)");
    checkBool(b.isNegative, true, "chained assignment: b is negative");

    BigInt neg("-5");
    neg = BigInt(0);
    checkEq(neg.number, "0",  "assign zero: digits become \"0\"");
    checkBool(neg.isNegative, false, "assign zero: isNegative cleared");

    cout << "--- toString ---\n";
    checkEq(BigInt(12345).toString(),  "12345",  "toString positive");
    checkEq(BigInt(-67890).toString(), "-67890", "toString negative");
    checkEq(BigInt(0).toString(),      "0",      "toString zero (no minus sign)");
    checkEq(BigInt("1").toString(),    "1",      "toString single digit");
    checkEq(BigInt("-1").toString(),   "-1",     "toString minus one");
    checkEq(BigInt("123456789012345678901234567890").toString(),
            "123456789012345678901234567890", "toString large positive");

    cout << "--- operator<< ---\n";
    {
        ostringstream oss;
        oss << BigInt(12345) << " " << BigInt(-67890) << " " << BigInt(0);
        checkEq(oss.str(), "12345 -67890 0", "operator<< positive, negative, zero");
    }
    {
        ostringstream oss;
        oss << BigInt("123456789012345678901234567890");
        checkEq(oss.str(), "123456789012345678901234567890", "operator<< large number");
    }
    {
        ostringstream oss;
        oss << BigInt("-1");
        checkEq(oss.str(), "-1", "operator<< minus one");
    }

    return r;
}

// ---------------------------------------------------------------------------
// BIGINT-22: unit tests for input stream operator>>
// ---------------------------------------------------------------------------
static TestResult runInputStreamTests()
{
    TestResult r;
    auto checkEq = [&](const string& actual, const string& expected, const char* desc) {
        if (actual == expected) { cout << "  PASS: " << desc << "\n"; ++r.passed; }
        else {
            cout << "  FAIL: " << desc << "\n"
                 << "        expected: \"" << expected << "\"\n"
                 << "        actual:   \"" << actual   << "\"\n";
            ++r.failed;
        }
    };
    auto checkBool = [&](bool actual, bool expected, const char* desc) {
        if (actual == expected) { cout << "  PASS: " << desc << "\n"; ++r.passed; }
        else {
            cout << "  FAIL: " << desc << "\n"
                 << "        expected: " << (expected ? "true" : "false") << "\n"
                 << "        actual:   " << (actual   ? "true" : "false") << "\n";
            ++r.failed;
        }
    };

    cout << "--- operator>> ---\n";

    BigInt a;
    istringstream("999") >> a;
    checkEq(a.number, "999", "\"999\" -> digits \"999\"");
    checkBool(a.isNegative, false, "\"999\" -> not negative");

    BigInt b;
    istringstream("-999") >> b;
    checkEq(b.number, "999", "\"-999\" -> digits \"999\"");
    checkBool(b.isNegative, true, "\"-999\" -> negative");

    BigInt c;
    istringstream("000123") >> c;
    checkEq(c.number, "123", "\"000123\" -> leading zeros stripped to \"123\"");
    checkBool(c.isNegative, false, "\"000123\" -> not negative");

    BigInt d;
    istringstream("0") >> d;
    checkEq(d.number, "0", "\"0\" -> digits \"0\"");
    checkBool(d.isNegative, false, "\"0\" -> not negative");

    return r;
}

// ---------------------------------------------------------------------------
// BIGINT-37: Addition tests (operator+= / operator+)
// ---------------------------------------------------------------------------
static TestResult runAdditionTests()
{
    TestResult r;
    auto checkEq = [&](const string& actual, const string& expected, const char* desc) {
        if (actual == expected) { cout << "  PASS: " << desc << "\n"; ++r.passed; }
        else {
            cout << "  FAIL: " << desc << "\n"
                 << "        expected: \"" << expected << "\"\n"
                 << "        actual:   \"" << actual   << "\"\n";
            ++r.failed;
        }
    };

    cout << "--- addition ---\n";
    // positive + positive
    checkEq((BigInt("123") + BigInt("456")).toString(), "579",
            "positive + positive: 123 + 456 = 579");
    // negative + negative
    checkEq((BigInt("-50") + BigInt("-30")).toString(), "-80",
            "negative + negative: -50 + -30 = -80");
    // positive + negative (positive wins)
    checkEq((BigInt("100") + BigInt("-40")).toString(), "60",
            "positive + negative: 100 + -40 = 60");
    // positive + negative (negative wins)
    checkEq((BigInt("40") + BigInt("-100")).toString(), "-60",
            "positive + negative: 40 + -100 = -60");
    // negative + positive
    checkEq((BigInt("-70") + BigInt("20")).toString(), "-50",
            "negative + positive: -70 + 20 = -50");
    // cancel to zero
    checkEq((BigInt("99") + BigInt("-99")).toString(), "0",
            "cancel to zero: 99 + -99 = 0");
    // carry across multiple digits
    checkEq((BigInt("999") + BigInt("1")).toString(), "1000",
            "carry across digits: 999 + 1 = 1000");
    checkEq((BigInt("999999999") + BigInt("1")).toString(), "1000000000",
            "carry across many digits: 999999999 + 1 = 1000000000");
    // large numbers
    checkEq((BigInt("123456789012345678901234567890") +
             BigInt("987654321098765432109876543210")).toString(),
            "1111111110111111111011111111100",
            "large positive + large positive");
    // add zero
    checkEq((BigInt("42") + BigInt("0")).toString(), "42", "n + 0 = n");
    checkEq((BigInt("0") + BigInt("-5")).toString(), "-5", "0 + (-5) = -5");

    return r;
}

// ---------------------------------------------------------------------------
// BIGINT-38: Subtraction tests (operator-= / operator-)
// ---------------------------------------------------------------------------
static TestResult runSubtractionTests()
{
    TestResult r;
    auto checkEq = [&](const string& actual, const string& expected, const char* desc) {
        if (actual == expected) { cout << "  PASS: " << desc << "\n"; ++r.passed; }
        else {
            cout << "  FAIL: " << desc << "\n"
                 << "        expected: \"" << expected << "\"\n"
                 << "        actual:   \"" << actual   << "\"\n";
            ++r.failed;
        }
    };

    cout << "--- subtraction ---\n";
    // equal numbers -> zero (no negative zero)
    checkEq((BigInt("55") - BigInt("55")).toString(), "0",
            "equal numbers: 55 - 55 = 0");
    // positive - larger positive -> negative
    checkEq((BigInt("30") - BigInt("100")).toString(), "-70",
            "positive - larger positive: 30 - 100 = -70");
    // negative - negative
    checkEq((BigInt("-10") - BigInt("-3")).toString(), "-7",
            "negative - negative: -10 - -3 = -7");
    checkEq((BigInt("-3") - BigInt("-10")).toString(), "7",
            "negative - negative (result positive): -3 - -10 = 7");
    // negative - positive
    checkEq((BigInt("-5") - BigInt("3")).toString(), "-8",
            "negative - positive: -5 - 3 = -8");
    // borrow logic
    checkEq((BigInt("1000") - BigInt("1")).toString(), "999",
            "borrow across digits: 1000 - 1 = 999");
    // subtract zero
    checkEq((BigInt("42") - BigInt("0")).toString(), "42", "n - 0 = n");

    return r;
}

// ---------------------------------------------------------------------------
// BIGINT-39: Multiplication tests (operator*= / operator*)
// ---------------------------------------------------------------------------
static TestResult runMultiplicationTests()
{
    TestResult r;
    auto checkEq = [&](const string& actual, const string& expected, const char* desc) {
        if (actual == expected) { cout << "  PASS: " << desc << "\n"; ++r.passed; }
        else {
            cout << "  FAIL: " << desc << "\n"
                 << "        expected: \"" << expected << "\"\n"
                 << "        actual:   \"" << actual   << "\"\n";
            ++r.failed;
        }
    };

    cout << "--- multiplication ---\n";
    // zero cases
    checkEq((BigInt("0") * BigInt("12345")).toString(), "0", "0 * n = 0");
    checkEq((BigInt("12345") * BigInt("0")).toString(), "0", "n * 0 = 0");
    // single-digit x multi-digit
    checkEq((BigInt("7") * BigInt("123")).toString(), "861", "7 * 123 = 861");
    checkEq((BigInt("9") * BigInt("9")).toString(), "81", "9 * 9 = 81");
    // negative x positive (sign XOR rule)
    checkEq((BigInt("-3") * BigInt("5")).toString(), "-15", "-3 * 5 = -15");
    checkEq((BigInt("-3") * BigInt("-5")).toString(), "15", "-3 * -5 = 15");
    // large numbers (page 10 expected output)
    checkEq((BigInt("12345678") * BigInt("87654321")).toString(),
            "1082152022374638", "12345678 * 87654321 = 1082152022374638");
    checkEq((BigInt("999") * BigInt("999")).toString(), "998001", "999 * 999 = 998001");

    return r;
}

// ---------------------------------------------------------------------------
// BIGINT-40: Division tests (operator/= / operator/)
// ---------------------------------------------------------------------------
static TestResult runDivisionTests()
{
    TestResult r;
    auto checkEq = [&](const string& actual, const string& expected, const char* desc) {
        if (actual == expected) { cout << "  PASS: " << desc << "\n"; ++r.passed; }
        else {
            cout << "  FAIL: " << desc << "\n"
                 << "        expected: \"" << expected << "\"\n"
                 << "        actual:   \"" << actual   << "\"\n";
            ++r.failed;
        }
    };
    auto checkThrows = [&](const char* desc) {
        try {
            BigInt a("5"); BigInt b("0"); a /= b;
            cout << "  FAIL: " << desc << " (no exception thrown)\n"; ++r.failed;
        } catch (const runtime_error&) {
            cout << "  PASS: " << desc << "\n"; ++r.passed;
        }
    };

    cout << "--- division ---\n";
    // divide by zero
    checkThrows("divide by zero throws runtime_error");
    // exact division
    checkEq((BigInt("100") / BigInt("5")).toString(), "20", "100 / 5 = 20");
    checkEq((BigInt("81") / BigInt("9")).toString(), "9", "81 / 9 = 9");
    // non-exact division (truncation toward zero)
    checkEq((BigInt("10") / BigInt("3")).toString(), "3", "10 / 3 = 3 (truncated)");
    checkEq((BigInt("7") / BigInt("2")).toString(), "3", "7 / 2 = 3 (truncated)");
    // divisor larger than dividend
    checkEq((BigInt("3") / BigInt("10")).toString(), "0", "3 / 10 = 0");
    // negative division (truncation toward zero)
    checkEq((BigInt("-10") / BigInt("3")).toString(), "-3", "-10 / 3 = -3 (toward zero)");
    checkEq((BigInt("10") / BigInt("-3")).toString(), "-3", "10 / -3 = -3 (toward zero)");
    checkEq((BigInt("-10") / BigInt("-3")).toString(), "3", "-10 / -3 = 3");
    // large numbers
    checkEq((BigInt("1082152022374638") / BigInt("12345678")).toString(),
            "87654321", "1082152022374638 / 12345678 = 87654321");

    return r;
}

// ---------------------------------------------------------------------------
// BIGINT-41: Modulus tests (operator%= / operator%)
// ---------------------------------------------------------------------------
static TestResult runModulusTests()
{
    TestResult r;
    auto checkEq = [&](const string& actual, const string& expected, const char* desc) {
        if (actual == expected) { cout << "  PASS: " << desc << "\n"; ++r.passed; }
        else {
            cout << "  FAIL: " << desc << "\n"
                 << "        expected: \"" << expected << "\"\n"
                 << "        actual:   \"" << actual   << "\"\n";
            ++r.failed;
        }
    };
    auto checkThrows = [&](const char* desc) {
        try {
            BigInt a("5"); BigInt b("0"); a %= b;
            cout << "  FAIL: " << desc << " (no exception thrown)\n"; ++r.failed;
        } catch (const runtime_error&) {
            cout << "  PASS: " << desc << "\n"; ++r.passed;
        }
    };

    cout << "--- modulus ---\n";
    // divide by zero
    checkThrows("modulus by zero throws runtime_error");
    // positive % positive
    checkEq((BigInt("10") % BigInt("3")).toString(), "1", "10 % 3 = 1");
    checkEq((BigInt("9") % BigInt("3")).toString(), "0", "9 % 3 = 0 (exact)");
    // negative % positive (sign follows dividend)
    checkEq((BigInt("-7") % BigInt("3")).toString(), "-1", "-7 % 3 = -1 (sign = dividend)");
    // positive % negative (sign follows dividend)
    checkEq((BigInt("7") % BigInt("-3")).toString(), "1", "7 % -3 = 1 (sign = dividend)");
    // dividend smaller than divisor
    checkEq((BigInt("3") % BigInt("10")).toString(), "3", "3 % 10 = 3");
    checkEq((BigInt("-3") % BigInt("10")).toString(), "-3", "-3 % 10 = -3");

    return r;
}

// ---------------------------------------------------------------------------
// BIGINT-78: Unary plus, unary minus, pre/post increment/decrement tests
// ---------------------------------------------------------------------------
TestResult runUnaryAndIncDecTests()
{
    TestResult r;
    auto check = [&](bool cond, const char* desc) {
        if (cond) { cout << "  PASS: " << desc << "\n"; ++r.passed; }
        else       { cout << "  FAIL: " << desc << "\n"; ++r.failed; }
    };

    cout << "\n--- Unary Plus ---\n";
    BigInt a(42);
    check((+a).toString() == "42",    "+42 == 42");
    BigInt b(-7);
    check((+b).toString() == "-7",    "+(-7) == -7");
    BigInt z(0);
    check((+z).toString() == "0",     "+0 == 0");

    cout << "\n--- Unary Minus ---\n";
    check((-a).toString() == "-42",   "-42 == -42");
    check((-b).toString() == "7",     "-(-7) == 7");
    check((-z).toString() == "0",     "-0 == 0");
    // double negation
    check((-(-a)).toString() == "42", "--42 == 42");

    cout << "\n--- Pre-increment (++x) ---\n";
    BigInt x(5);
    BigInt &ref = ++x;
    check(x.toString() == "6",        "++5 yields 6");
    check(ref.toString() == "6",      "++x returns *this (value 6)");
    BigInt neg(-1);
    ++neg;
    check(neg.toString() == "0",      "++-1 yields 0");
    BigInt negTwo(-2);
    ++negTwo;
    check(negTwo.toString() == "-1",  "++-2 yields -1");

    cout << "\n--- Post-increment (x++) ---\n";
    BigInt p(10);
    BigInt old = p++;
    check(old.toString() == "10",     "x++ returns old value 10");
    check(p.toString() == "11",       "x++ increments to 11");

    cout << "\n--- Pre-decrement (--x) ---\n";
    BigInt d(3);
    BigInt &dref = --d;
    check(d.toString() == "2",        "--3 yields 2");
    check(dref.toString() == "2",     "--x returns *this (value 2)");
    BigInt zero2(0);
    --zero2;
    check(zero2.toString() == "-1",   "--0 yields -1");

    cout << "\n--- Post-decrement (x--) ---\n";
    BigInt q(7);
    BigInt qold = q--;
    check(qold.toString() == "7",     "x-- returns old value 7");
    check(q.toString() == "6",        "x-- decrements to 6");

    cout << "\n--- Inc/Dec across zero ---\n";
    BigInt crossPos(0);
    crossPos++;
    check(crossPos.toString() == "1", "0++ yields 1");
    BigInt crossNeg(0);
    crossNeg--;
    check(crossNeg.toString() == "-1","0-- yields -1");

    return r;
}

// ---------------------------------------------------------------------------
// BIGINT-79: Equality and ordering comparison operator tests
// ---------------------------------------------------------------------------
TestResult runComparisonTests()
{
    TestResult r;
    auto check = [&](bool cond, const char* desc) {
        if (cond) { cout << "  PASS: " << desc << "\n"; ++r.passed; }
        else       { cout << "  FAIL: " << desc << "\n"; ++r.failed; }
    };

    BigInt pos5(5), pos5b(5), neg5(-5), pos3(3), neg3(-3), zero(0), zero2(0);
    BigInt big("123456789012345678901234567890");
    BigInt bigb("123456789012345678901234567890");
    BigInt bigger("999999999999999999999999999999");

    cout << "\n--- operator== ---\n";
    check(pos5 == pos5b,   "5 == 5");
    check(zero == zero2,   "0 == 0");
    check(big == bigb,     "large == large");
    check(!(pos5 == neg5), "5 != -5");
    check(!(pos5 == pos3), "5 != 3");

    cout << "\n--- operator!= ---\n";
    check(pos5 != neg5,    "5 != -5");
    check(pos5 != pos3,    "5 != 3");
    check(!(pos5 != pos5b),"!(5 != 5)");

    cout << "\n--- operator< ---\n";
    check(pos3 < pos5,     "3 < 5");
    check(neg5 < pos5,     "-5 < 5");
    check(neg5 < neg3,     "-5 < -3");
    check(neg5 < zero,     "-5 < 0");
    check(zero < pos5,     "0 < 5");
    check(!(pos5 < pos3),  "!(5 < 3)");
    check(!(pos5 < pos5b), "!(5 < 5)");
    check(big < bigger,    "large < larger");

    cout << "\n--- operator<= ---\n";
    check(pos3 <= pos5,    "3 <= 5");
    check(pos5 <= pos5b,   "5 <= 5");
    check(neg5 <= neg3,    "-5 <= -3");
    check(!(pos5 <= pos3), "!(5 <= 3)");

    cout << "\n--- operator> ---\n";
    check(pos5 > pos3,     "5 > 3");
    check(pos5 > neg5,     "5 > -5");
    check(neg3 > neg5,     "-3 > -5");
    check(zero > neg5,     "0 > -5");
    check(!(pos3 > pos5),  "!(3 > 5)");
    check(!(pos5 > pos5b), "!(5 > 5)");

    cout << "\n--- operator>= ---\n";
    check(pos5 >= pos3,    "5 >= 3");
    check(pos5 >= pos5b,   "5 >= 5");
    check(neg3 >= neg5,    "-3 >= -5");
    check(!(pos3 >= pos5), "!(3 >= 5)");

    cout << "\n--- Zero comparisons ---\n";
    BigInt negZero(0);
    check(zero == negZero,  "0 == 0 (no negative zero)");
    check(!(zero < negZero),"!(0 < 0)");

    return r;
}

// ---------------------------------------------------------------------------
// BIGINT-81: Edge-case audit — all common mistakes from page 12 of the guide
// ---------------------------------------------------------------------------
TestResult runEdgeCaseAuditTests()
{
    TestResult r;
    auto check = [&](bool cond, const char* desc) {
        if (cond) { cout << "  PASS: " << desc << "\n"; ++r.passed; }
        else       { cout << "  FAIL: " << desc << "\n"; ++r.failed; }
    };

    cout << "\n--- Normalization: leading zeros stripped ---\n";
    BigInt fromStr("000123");
    check(fromStr.toString() == "123",       "\"000123\" -> \"123\"");
    BigInt product = BigInt(10) * BigInt(10);
    check(product.toString() == "100",       "10 * 10 -> \"100\" (no leading zeros)");

    cout << "\n--- Negative zero never produced ---\n";
    BigInt a(5), b(5);
    BigInt diff = a - b;
    check(diff.toString() == "0",            "5 - 5 == 0");
    check(!diff.isNegative,                  "5 - 5 has isNegative == false");
    BigInt c(-5), d(-5);
    BigInt diff2 = c - d;
    check(diff2.toString() == "0",           "-5 - (-5) == 0");
    check(!diff2.isNegative,                 "-5 - (-5) has isNegative == false");
    BigInt neg = -BigInt(0);
    check(neg.toString() == "0",             "-0 == 0");
    check(!neg.isNegative,                   "-0 has isNegative == false");

    cout << "\n--- No intermediate int overflow in multiplication ---\n";
    BigInt big1("999999999999999999");
    BigInt big2("999999999999999999");
    BigInt bigProd = big1 * big2;
    check(bigProd.toString() == "999999999999999998000000000000000001",
          "999999999999999999^2 is correct");

    cout << "\n--- Wrong sign after subtracting equal numbers ---\n";
    BigInt x(100), y(100);
    BigInt res = x - y;
    check(res.toString() == "0" && !res.isNegative, "100 - 100 == 0, not negative");

    cout << "\n--- Division truncation toward zero (not floor) ---\n";
    // -7 / 2 should be -3 (truncation), not -4 (floor)
    BigInt neg7(-7), two(2);
    BigInt divResult = neg7 / two;
    check(divResult.toString() == "-3",      "-7 / 2 == -3 (truncation toward zero)");
    // 7 / -2 should be -3 as well
    BigInt pos7(7), neg2(-2);
    BigInt divResult2 = pos7 / neg2;
    check(divResult2.toString() == "-3",     "7 / -2 == -3 (truncation toward zero)");

    cout << "\n--- Modulus sign follows dividend ---\n";
    // -7 % 3 should be -1 (sign of dividend -7)
    BigInt neg7b(-7), three(3);
    BigInt modResult = neg7b % three;
    check(modResult.toString() == "-1",      "-7 % 3 == -1 (sign follows dividend)");
    // 7 % -3 should be 1 (sign of dividend 7)
    BigInt pos7b(7), neg3(-3);
    BigInt modResult2 = pos7b % neg3;
    check(modResult2.toString() == "1",      "7 % -3 == 1 (sign follows dividend)");

    cout << "\n--- Division by zero throws ---\n";
    bool threw = false;
    try { BigInt(5) / BigInt(0); } catch (const runtime_error&) { threw = true; }
    check(threw, "5 / 0 throws runtime_error");
    bool threw2 = false;
    try { BigInt(5) % BigInt(0); } catch (const runtime_error&) { threw2 = true; }
    check(threw2, "5 % 0 throws runtime_error");

    cout << "\n--- Very large number arithmetic ---\n";
    BigInt vl1("99999999999999999999999999999999999999999999999999");
    BigInt vl2("1");
    BigInt vlSum = vl1 + vl2;
    check(vlSum.toString() == "100000000000000000000000000000000000000000000000000",
          "50-digit number + 1 correct");
    BigInt vlDiff = vlSum - vl2;
    check(vlDiff == vl1, "round-trip: (vl1 + 1) - 1 == vl1");

    return r;
}

// ---------------------------------------------------------------------------
// BIGINT-26: lightweight test driver — runs all suites and
//            prints a per-suite summary plus an overall PASS/FAIL line.
// ---------------------------------------------------------------------------
int main()
{
    cout << "=== BigInt Full Test Harness ===\n\n";

    struct Suite { const char* name; TestResult result; };
    Suite suites[] = {
        { "Normalization (basic)",           runNormalizationTests()        },
        { "Normalization (edge cases)",      runNormalizationEdgeCaseTests()},
        { "Constructors",                    runConstructorTests()          },
        { "Assignment & Output",             runAssignmentAndOutputTests()  },
        { "Input Stream (operator>>)",       runInputStreamTests()          },
        { "Addition  (BIGINT-37)",           runAdditionTests()             },
        { "Subtraction (BIGINT-38)",         runSubtractionTests()          },
        { "Multiplication (BIGINT-39)",      runMultiplicationTests()       },
        { "Division (BIGINT-40)",            runDivisionTests()             },
        { "Modulus (BIGINT-41)",             runModulusTests()              },
        { "Unary & Inc/Dec (BIGINT-78)",     runUnaryAndIncDecTests()       },
        { "Comparisons (BIGINT-79)",         runComparisonTests()           },
        { "Edge-Case Audit (BIGINT-81)",     runEdgeCaseAuditTests()        },
    };

    cout << "\n=== Suite Summary ===\n";
    int totalPassed = 0, totalFailed = 0;
    for (auto& s : suites) {
        totalPassed += s.result.passed;
        totalFailed += s.result.failed;
        cout << "  [" << (s.result.failed == 0 ? "OK  " : "FAIL")
             << "] " << s.name
             << " (" << s.result.passed << " passed";
        if (s.result.failed > 0)
            cout << ", " << s.result.failed << " failed";
        cout << ")\n";
    }

    cout << "\n=== TOTAL: " << totalPassed << " passed, "
         << totalFailed << " failed";
    if (totalFailed == 0)
        cout << " — ALL PASS";
    cout << " ===\n";

    return totalFailed == 0 ? 0 : 1;
}
