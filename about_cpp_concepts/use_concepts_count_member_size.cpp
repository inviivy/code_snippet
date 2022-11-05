#include <cctype>
#include <concepts>
#include <iostream>
#include <cassert>

struct UniversalType
{
    template <typename T>
    operator T() { return T{}; }
};

template <typename T, typename... Args>
consteval std::size_t member_count_impl()
{
    if constexpr (requires {
                      T{{Args{}}..., {UniversalType{}}};
                  } == true)
    {
        return member_count_impl<T, Args..., UniversalType>();
    }
    else
    {
        return sizeof...(Args);
    }
}

struct MyType
{
    int a;
    int b;
    int c;
};

static_assert(member_count_impl<MyType>() == 3, "xx");

// g++ test.cpp --std=c++2a
int main()
{
}