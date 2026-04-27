#include <expected>
#include <print>
#include <string_view>

#define PARSE_ERRORS(MACRO)                      \
    MACRO(bad_format, "expected YYYY-MM-DD")     \
    MACRO(year_digits, "invalid year digits")    \
    MACRO(month_digits, "invalid month digits")  \
    MACRO(day_digits, "invalid day digits")      \
    MACRO(year_range, "year must be 1900..9999") \
    MACRO(month_range, "month must be 1..12")    \
    MACRO(day_range, "day out of range for month/year")

enum struct birthdate_error
{
#define XXX(name, info) name,
    PARSE_ERRORS(XXX)
#undef XXX
};

static constexpr std::string_view to_string(birthdate_error e)
{
    switch (e) {
#define XXX(name, info) \
    case birthdate_error::name: return info;
        PARSE_ERRORS(XXX)
#undef XXX
    }
    return "unknown parse error";
};

#undef PARSE_ERRORS

////////////////////////////////////////////////////////////////////////////////

class birthdate
{
    int year_;
    int month_;
    int day_;

public:
    static constexpr std::expected<birthdate, birthdate_error>
    parse(std::string_view date_str) noexcept
    {
        if (date_str.size() != 10 || date_str[4] != '-' || date_str[7] != '-') {
            return std::unexpected(birthdate_error::bad_format);
        }

        auto in_range = [](int val, int min, int max) static {
            return val >= min && val <= max;
        };

        const auto year = parse_digits(date_str.substr(0, 4));
        if (year < 0) return std::unexpected(birthdate_error::year_digits);
        if (!in_range(year, 1900, 9999))
            return std::unexpected(birthdate_error::year_range);

        const auto month = parse_digits(date_str.substr(5, 2));
        if (month < 0) return std::unexpected(birthdate_error::month_digits);
        if (!in_range(month, 1, 12))
            return std::unexpected(birthdate_error::month_range);

        const auto day = parse_digits(date_str.substr(8, 2));
        if (day < 0) return std::unexpected(birthdate_error::day_digits);
        if (int maxd = days_in_month(year, month); !in_range(day, 1, maxd))
            return std::unexpected(birthdate_error::day_range);

        return birthdate(year, month, day);
    }

    constexpr int year() const noexcept { return year_; }
    constexpr int month() const noexcept { return month_; }
    constexpr int day() const noexcept { return day_; }

private:
    constexpr birthdate(int y, int m, int d) noexcept
    : year_(y), month_(m), day_(d)
    {
    }

    // Return negative integer to signal error and avoid inclusion of <optional>
    // Manual parsing for this simple case and avoid inclusion of <charconv>
    static constexpr int parse_digits(std::string_view digits) noexcept
    {
        int ret = 0;
        for (char c : digits) {
            if (c < '0' || c > '9') return -1;
            ret = ret * 10 + (c - '0');
        }
        return ret;
    }

    static constexpr int days_in_month(int year, int month) noexcept
    {
        static constexpr int days[12] = {31, 28, 31, 30, 31, 30,
                                         31, 31, 30, 31, 30, 31};
        //
        auto is_leap = [](int y) static {
            return (y % 400 == 0) || ((y % 4 == 0) && (y % 100 != 0));
        };
        return (month == 2 && is_leap(year)) ? 29 : days[month - 1];
    }
};

////////////////////////////////////////////////////////////////////////////////

int main()
{
    using namespace std::string_view_literals;
    for (auto date : {"2026-04-17"sv, "2026-02-29"sv}) {
        if (const auto res = birthdate::parse(date); res) {
            std::println("Parse succeeded. Input: {}. Date: {:04}-{:02}-{:02}.",
                         date, res->year(), res->month(), res->day());
        } else {
            std::println("Parse failed. Input: {}. Error: {}.", date,
                         to_string(res.error()));
        }
    }
    return 0;
}
