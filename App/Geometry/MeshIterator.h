#ifndef MESHITERATOR_H
#define MESHITERATOR_H

#include <functional>

namespace blithe
{
    ///
    /// \brief A templated class for iterating or accessing over triangle (and other facets, no
    ///        pun intended) of a Mesh.
    ///
    template <typename T, typename MeshType>
    class MeshIterator
    {
    public:
        using AccessorFunc = std::function<T(const MeshType&, size_t)>;

        ///
        /// \brief Constructor for the iterator
        ///
        /// \param _mesh         - Const ref to the Mesh
        /// \param _index        - Initial value for the current index
        /// \param _accessorFunc - The accessor function to use
        ///
        MeshIterator(const MeshType& _mesh, size_t _index, AccessorFunc _accessorFunc)
            : m_mesh(_mesh), m_index(_index), m_accessorFunc(_accessorFunc)
        {
        }

        ///
        /// \brief operator * for the iterator
        ///
        /// \return The iterator value
        ///
        T operator*() const
        {
            return m_accessorFunc(m_mesh, m_index);
        }

        ///
        /// \brief operator ++ for the iterator
        ///
        /// \return Ref to the incremented iterator
        ///
        MeshIterator& operator++()
        {
            ++m_index;
            return *this;
        }

        ///
        /// \brief operator != for the iterator
        ///
        /// \param _other - Other iterator to compare against
        ///
        /// \return True if the iterators are equal, else false
        ///
        bool operator!=(const MeshIterator& _other) const
        {
            return m_index != _other.m_index;
        }

    private:
        const MeshType& m_mesh;       //!< Const ref the Mesh to be iterated over
        size_t m_index;               //!< Current index of the iterator
        AccessorFunc m_accessorFunc;  //!< Accessor function to use
    };
}

#endif //MESHITERATOR_H
