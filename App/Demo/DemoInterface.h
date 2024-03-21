#ifndef DEMOINTERFACE_H
#define DEMOINTERFACE_H

#include <string>

namespace BLE
{
    class UIData;

    /*!
     * \brief Interface that demos implement
     */
    class DemoInterface
    {
    public:
        virtual ~DemoInterface() = default;
        virtual void OnInit() = 0;                        //!< Called when the demo is initialized
        virtual void OnRender(const UIData& _uiData,
                              const UIData* _defaultViewPortUIData = nullptr) = 0; //!< Called every frame to render the demo
        virtual void OnDrawUI() = 0;                      //!< Called every frame to render ImGui components
        virtual bool UsesDefaultScene() const = 0;        //!< Whether this demo would like to render to default scene
    };

    /*!
     * \brief Factory for creating demos
     */
    class DemoFactory
    {
    public:
        virtual ~DemoFactory() = default;
        virtual std::string GetName() const = 0;   //!< Gets the name of the demo
        virtual DemoInterface* CreateDemo() = 0; //!< Creates the demo
    };
}

#endif //DEMOINTERFACE_H