#include "BlitheDemoFactories.h"
#include "TriangleDemo.h"
#include "CubeDemo.h"
#include "InstancedCubeDemo.h"
#include "GrassDemo.h"
#include "SimpleBSPDemo.h"

namespace blithe
{
#define ADD_DEMO(DemoClass) \
    m_demoFactories.push_back(new blithe::DemoClass##Factory())

    ///
    /// \brief Default constructor. Creates all the demo factories.
    ///
    BlitheDemoFactories::BlitheDemoFactories()
    {
        ADD_DEMO(TriangleDemo);
        ADD_DEMO(CubeDemo);
        ADD_DEMO(InstancedCubeDemo);
        ADD_DEMO(GrassDemo);
        ADD_DEMO(SimpleBSPDemo);
    }

    ///
    /// \brief Deletes all the demo factories.
    ///
    BlitheDemoFactories::~BlitheDemoFactories()
    {
        for (DemoFactory* fac : m_demoFactories)
        {
            delete fac;
        }
        m_demoFactories.clear();
    }
}
