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

    int typeToSize(const Type& type)
    {
        switch(type.kind)
        {
            case TypeKind::tpInt:
                return 4;
            case TypeKind::tpChar:
                return 1;
            case TypeKind::tpPoint:
                return 8;
            case TypeKind::tpArray:
                return typeToSize(*type.baseType) * type.size;
            default:
                return 0;
        }
    }
}
