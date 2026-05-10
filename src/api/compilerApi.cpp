#include "compilerApi.hpp"

std::vector<Error> analyse(const std::string& source)
{
    std::vector<Error> errors;

    if (source.find("foo") != std::string::npos)
    {
        errors.push_back({
            "Unexpected token 'foo'",
            0, 0, 3
        });
    }

    if (source.find("bar") != std::string::npos)
    {
        errors.push_back({
            "Unknown symbol 'bar'",
            1, 0, 3
        });
    }

    return errors;
}