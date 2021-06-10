#pragma once
#include <iostream>
#include <cstdint>

// Defining enum Gender
enum class LabjackPortType { Analog, Digital, DigitalState };



//// enumeration types (both scoped and unscoped) can have overloaded operators
//std::ostream& operator<<(std::ostream& os, color c)
//{
//    switch (c)
//    {
//    case red: os << "red";    break;
//    case yellow: os << "yellow"; break;
//    case green: os << "green";  break;
//    case blue: os << "blue";   break;
//    default: os.setstate(std::ios_base::failbit);
//    }
//    return os;
//}
//
//std::ostream& operator<<(std::ostream& os, altitude al)
//{
//    return os << static_cast<char>(al);
//}
