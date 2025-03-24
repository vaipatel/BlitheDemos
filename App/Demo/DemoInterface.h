#ifndef DEMOINTERFACE_H
#define DEMOINTERFACE_H

#include <string>

namespace blithe
{
    class UIData;

    /*!
     * \brief Interface that demos implement
     */
    class DemoInterface
    {
    public:
        virtual ~DemoInterface() = default;
        virtual void OnInit() = 0;                                            //!< Called when the demo is initialized
        virtual void OnRender(double _deltaTimeS, const UIData& _uiData) = 0; //!< Called every frame to render the demo
        virtual void OnDrawUI() = 0;                                          //!< Called every frame to render ImGui components
        virtual bool UsesStandardViewPort() const = 0;                        //!< Whether this demo would like to render to the standardly provided viewport
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

#define DECLARE_DEMO(DemoClass, DemoName)                               \
    class DemoClass##Factory : public DemoFactory                       \
    {                                                                   \
    public:                                                             \
        ~DemoClass##Factory() override {}                               \
        std::string GetName() const override { return DemoName; }       \
        DemoInterface* CreateDemo() override { return new DemoClass;}   \
    }
}

#endif //DEMOINTERFACE_H
