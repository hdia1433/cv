#include "token.hpp"

Coordinate Token::operator->() const
{
    return location;
}