#include <iostream>
#include <sstream>
#include <string>
using namespace std;

// Aggregated result from a single test suite.
struct TestResult { int passed = 0, failed = 0; };

// Forward declarations (BIGINT-26: separate test blocks)
static TestResult runNormalizationTests();
static TestResult runNormalizationEdgeCaseTests();
static TestResult runConstructorTests();
static TestResult runAssignmentAndOutputTests();
static TestResult runInputStreamTests();

class BigInt
{
    string number;   // Stores the number as a string
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
        if(str[0] == '-'){
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
        if (this != &other)
        {
            number = other.number;
            isNegative = other.isNegative;
        }
        return *this;
    }

    // Unary negation operator (-x)
    BigInt operator-() const
    {
        BigInt result;
        // TODO: Implement negation logic
        return result;
    }

    // Unary plus operator (+x)
    BigInt operator+() const
    {
        BigInt result;
        // TODO: Implement this operator
        return result;
    }

    // Addition assignment operator (x += y)
    BigInt &operator+=(const BigInt &other)
    {
        // TODO: Implement this operator
        return *this;
    }

    // Subtraction assignment operator (x -= y)
    BigInt &operator-=(const BigInt &other)
    {
        // TODO: Implement this operator
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
        // TODO: Implement this operator
        return *this;
    }

    // Post-increment operator (x++)
    BigInt operator++(int)
    {
        BigInt temp;
        // TODO: Implement this operator
        return temp;
    }

    // Pre-decrement operator (--x)
    BigInt &operator--()
    {
        // TODO: Implement this operator
        return *this;
    }

    // Post-decrement operator (x--)
    BigInt operator--(int)
    {
        BigInt temp;
        // TODO: Implement this operator
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
    // TODO: Implement this operator
    return false;
}

// Inequality comparison operator (x != y)
bool operator!=(const BigInt &lhs, const BigInt &rhs)
{
    // TODO: Implement this operator
    return false;
}

// Less-than comparison operator (x < y)
bool operator<(const BigInt &lhs, const BigInt &rhs)
{
    // TODO: Implement this operator
    return false;
}

// Less-than-or-equal comparison operator (x <= y)
bool operator<=(const BigInt &lhs, const BigInt &rhs)
{
    // TODO: Implement this operator
    return false;
}

// Greater-than comparison operator (x > y)
bool operator>(const BigInt &lhs, const BigInt &rhs)
{
    // TODO: Implement this operator
    return false;
}

// Greater-than-or-equal comparison operator (x >= y)
bool operator>=(const BigInt &lhs, const BigInt &rhs)
{
    // TODO: Implement this operator
    return false;
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
//   - Multiple leading zeros in positive strings
//   - Negative zero variants ("-0", "-00", "-000123" with zero digits)
//   - Very long zero strings
//   - Mixed: "-007" should become "-7", not "-007"
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
        // Independence: modifying src should not affect dst
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

    // copy assignment
    BigInt src(42);
    BigInt dst;
    dst = src;
    checkEq(dst.number, "42",  "copy assignment: value copied");
    checkBool(dst.isNegative, false, "copy assignment: sign copied");

    // self-assignment guard
    BigInt self("999");
    self = self;
    checkEq(self.number, "999", "self-assignment: value unchanged");
    checkBool(self.isNegative, false, "self-assignment: sign unchanged");

    // chained assignment
    BigInt a, b;
    a = b = BigInt("-7");
    checkEq(a.number, "7",  "chained assignment: a gets -7 (digits)");
    checkBool(a.isNegative, true, "chained assignment: a is negative");
    checkEq(b.number, "7",  "chained assignment: b gets -7 (digits)");
    checkBool(b.isNegative, true, "chained assignment: b is negative");

    // assigning zero clears isNegative
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
// BIGINT-22 / BIGINT-27: unit tests for input stream operator>>
// BIGINT-29: checkEq shows expected vs actual on failure
// ---------------------------------------------------------------------------
static TestResult runInputStreamTests()
{
    TestResult r;
    auto check = [&](bool cond, const char* desc) {
        if (cond) { cout << "  PASS: " << desc << "\n"; ++r.passed; }
        else       { cout << "  FAIL: " << desc << "\n"; ++r.failed; }
    };

    cout << "--- operator>> ---\n";

    BigInt a;
    istringstream("999") >> a;
    check(a.number == "999" && !a.isNegative, "\"999\" -> number=999, positive");

    BigInt b;
    istringstream("-999") >> b;
    check(b.number == "999" && b.isNegative, "\"-999\" -> number=999, negative");

    BigInt c;
    istringstream("000123") >> c;
    check(c.number == "123" && !c.isNegative, "\"000123\" -> leading zeros stripped to 123");

    BigInt d;
    istringstream("0") >> d;
    check(d.number == "0" && !d.isNegative, "\"0\" -> number=0, non-negative");

    return r;
}

// ---------------------------------------------------------------------------
// BIGINT-26: lightweight test driver — runs all Sprint 1 suites and
//            prints a per-suite summary plus an overall PASS/FAIL line.
// ---------------------------------------------------------------------------
int main()
{
    cout << "=== BigInt Sprint 1 Test Harness ===\n\n";

    struct Suite { const char* name; TestResult result; };
    Suite suites[] = {
        { "Normalization (basic)",      runNormalizationTests()        },
        { "Normalization (edge cases)", runNormalizationEdgeCaseTests()},
        { "Constructors",               runConstructorTests()          },
        { "Assignment & Output",        runAssignmentAndOutputTests()  },
        { "Input stream (>>)",          runInputStreamTests()          },
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
