#ifndef RENDEROBJECT_H
#define RENDEROBJECT_H

namespace blithe
{
    ///
    /// \brief Encapsulates Render commands queued up by the Game thread and executed in the Main
    ///        (Render) thread.
    ///
    ///        These will be placement newed with memory from a FrameAllocator to avoid hitting the
    ///        heap every frame.
    ///
    class RenderObject
    {
    public:
        virtual ~RenderObject() = default;

        virtual void Render() = 0; //!< Perform the actual GL commands
    };
}
#endif // RENDEROBJECT_H
