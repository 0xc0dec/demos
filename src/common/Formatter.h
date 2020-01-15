#pragma once

#include <sstream>

#define FMT(...) Formatter()(__VA_ARGS__)

class Formatter final
{
public:
    auto operator()() const
    {
        return buf_.str();
    }

    template <class TFirst, class... TRest>
    auto operator()(TFirst first, TRest... rest)
    {
        buf_ << first;
        return operator()(rest...);
    }

private:
    std::ostringstream buf_;
};
