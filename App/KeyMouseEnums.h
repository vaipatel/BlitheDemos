#ifndef KEYMOUSEENUMS_H
#define KEYMOUSEENUMS_H

#include <array>
#include <stddef.h>

namespace blithe
{
    enum class enPressedKey : int
    {
        KEY_W,
        KEY_A,
        KEY_S,
        KEY_D,
        KEY_ESCAPE
    };

    ///
    /// \brief Supported mouse buttons for drag tracking.
    /// \note Please update enMouseButtonVec if this is updated.
    ///
    enum class enMouseButton : int
    {
        Left = 0,
        Middle,
        Right,
        // --- Add new buttons above this --
        COUNT
    };

    ///
    /// \brief Vector of enMouseButton values, so they can be looped over in a for loop.
    /// \note This should be updated when enMouseButton is updated.
    ///
    constexpr std::array<enMouseButton, static_cast<size_t>(enMouseButton::COUNT)> enMouseButtonVec
    {
        enMouseButton::Left,
        enMouseButton::Middle,
        enMouseButton::Right
    };
}

#endif // KEYMOUSEENUMS_H
