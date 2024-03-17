#ifndef IMDEMOINTERFACE_H
#define IMDEMOINTERFACE_H

#include <string>

namespace IME
{
    /*!
     * \brief Interface that demos implement
     */
    class ImDemoInterface
    {
    public:
        virtual ~ImDemoInterface() = default;
        virtual void OnInit() = 0;                //!< Called when the demo is initialized
        virtual void OnRender(float _aspect) = 0; //!< Called every frame to render the demo
        virtual void OnImGuiRender() = 0;         //!< Called every frame to render ImGui components
    };

    /*!
     * \brief Factory for creating demos
     */
    class ImDemoFactory
    {
    public:
        virtual ~ImDemoFactory() = default;
        virtual std::string GetName() const = 0;   //!< Gets the name of the demo
        virtual ImDemoInterface* CreateDemo() = 0; //!< Creates the demo
    };
}

#endif //IMDEMOINTERFACE_H