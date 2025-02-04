#ifndef BLITHESHARED_H
#define BLITHESHARED_H

namespace blithe
{
    ///
    /// \brief Deletes \p _x and sets it to nullptr.
    ///
    /// \note  It seems this is considered silly, but I think it's helpful if _x is a cached value
    ///        that must be changed when some state has changed.
    ///
    ///        For eg: If _x, but state has changed, delete _x. If no _x, create _x based on state.
    ///
    /// \param _x - Reference to pointer to object that is to be deleted
    ///
    template<typename T>
    void DeleteAndNull(T*& _x)
    {
        // We can skip the nullptr check because apparently c++ does it for you in like this one
        // instance.
        delete _x;
        _x = nullptr;
    }

    //! Macro for suppressing unused function arg warnings
    #define UNUSED_ARG(arg) ((void)(arg))
}

#endif // BLITHESHARED_H
