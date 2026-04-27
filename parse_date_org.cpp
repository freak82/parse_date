#include <expected>
#include <iostream>
#include <string_view>

enum class ParseError
{
    BadFormat,
    YearDigits,
    MonthDigits,
    DayDigits,
    YearRange,
    MonthRange,
    DayRange
};

// could probably simplify this with an enum wrapper like magic enum
const char* to_string(ParseError e)
{
    switch (e) {
    case ParseError::BadFormat: return "expected YYYY-MM-DD";
    case ParseError::YearDigits: return "invalid year digits";
    case ParseError::MonthDigits: return "invalid month digits";
    case ParseError::DayDigits: return "invalid day digits";
    case ParseError::YearRange: return "year must be 1900..9999";
    case ParseError::MonthRange: return "month must be 1..12";
    case ParseError::DayRange: return "day out of range for month/year";
    }
    return "unknown parse error";
}

class Birthdate
{
public:
    static std::expected<Birthdate, ParseError> parse(std::string_view s)
    {
        if (s.size() != 10 || s[4] != '-' || s[7] != '-') {
            return std::unexpected(ParseError::BadFormat);
        }

        std::expected<int, ParseError> y =
            parse_n_digits(s, 0, 4, ParseError::YearDigits);
        if (!y) return std::unexpected(y.error());

        std::expected<int, ParseError> m =
            parse_n_digits(s, 5, 2, ParseError::MonthDigits);
        if (!m) return std::unexpected(m.error());

        std::expected<int, ParseError> d =
            parse_n_digits(s, 8, 2, ParseError::DayDigits);
        if (!d) return std::unexpected(d.error());

        if (*y < 1900 || *y > 9999)
            return std::unexpected(ParseError::YearRange);
        if (*m < 1 || *m > 12) return std::unexpected(ParseError::MonthRange);

        const int maxd = days_in_month(*y, *m);
        if (*d < 1 || *d > maxd) return std::unexpected(ParseError::DayRange);

        return Birthdate(*y, *m, *d);
    }

    int year() const noexcept { return y_; }
    int month() const noexcept { return m_; }
    int day() const noexcept { return d_; }

private:
    Birthdate(int y, int m, int d) : y_(y), m_(m), d_(d) {}

    static std::expected<int, ParseError> parse_n_digits(std::string_view s,
                                                         std::size_t pos,
                                                         std::size_t n,
                                                         ParseError on_error)
    {
        int value = 0;
        for (std::size_t i = 0; i < n; ++i) {
            const char c = s[pos + i];
            if (c < '0' || c > '9') return std::unexpected(on_error);
            value = value * 10 + (c - '0');
        }
        return value;
    }

    static bool is_leap(int y) noexcept
    {
        return (y % 400 == 0) || ((y % 4 == 0) && (y % 100 != 0));
    }

    static int days_in_month(int y, int m) noexcept
    {
        static constexpr int dim[12] = {31, 28, 31, 30, 31, 30,
                                        31, 31, 30, 31, 30, 31};
        return (m == 2 && is_leap(y)) ? 29 : dim[m - 1];
    }

    int y_, m_, d_;
};

int main()
{
    using namespace std::string_view_literals;
    for (auto date : {"2026-04-17"sv, "2026-02-29"sv}) {
        if (const auto res = Birthdate::parse(date); res) {
            std::cout << res->year() << "-" << res->month() << "-" << res->day()
                      << "\n";
        } else {
            std::cout << "Parse failed: " << to_string(res.error()) << "\n";
        }
    }
    return 0;
}
