#include "helpers.hpp"


namespace helpers
{
    bool isInt(std::string str)
    {
        try
        {
            std::stoi(str);
            return true;
        }
        catch(const std::exception& e)
        {
            return false;
        }
    }

    bool isFloat(std::string str)
    {
        try
        {
            std::stof(str);
            return true;
        }
        catch(const std::exception& e)
        {
            return false;
        }
        
    }

    bool isDouble(std::string str)
    {
        try
        {
            std::stod(str);
            return true;
        }
        catch(const std::exception& e)
        {
            return false;
        }
        
    }

    bool isNumber(std::string str)
    {
        if(isInt(str))
        {
            return true;
        }
        else if(isFloat(str))
        {
            return true;
        }
        else if(isDouble(str))
        {
            return true;
        }
        else
        {
            return false;
        }
    }

    int typeToSize(Type type)
    {
        switch(type.kind)
        {
            case TypeKind::tpInt:
                return 4;
            case TypeKind::tpChar:
                return 1;
            default:
                return 0;
        }
    }
}