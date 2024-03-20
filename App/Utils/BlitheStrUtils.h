#ifndef BLITHESTRUTILS_H
#define BLITHESTRUTILS_H

#include <iomanip>
#include <sstream>
#include <string>

namespace BLE
{
    /*!
    * \brief Returns the string representing the input integer _i in hex.
    *
    * \param _i - Input integer
    *
    * \return string version of _i in hex. Prefixed with 0x.
    */
    template<typename T>
    std::string IntToHex(T _i)
    {
        std::stringstream stream;
        stream << "0x"
            << std::setfill('0') << std::setw(sizeof(T) * 2)
            << std::hex << _i;
        return stream.str();
    }
}

#endif // BLITHESTRUTILS_H
