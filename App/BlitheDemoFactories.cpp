#include "BlitheDemoFactories.h"
#include "TriangleDemo.h"
#include "CubeDemo.h"
#include "InstancedCubeDemo.h"
#include "GrassDemo.h"
#include "SimpleBSPDemo.h"

namespace blithe
{
    ///
    /// \brief Default constructor. Creates all the demo factories.
    ///
    BlitheDemoFactories::BlitheDemoFactories()
    {
        m_demoFactories.push_back(new blithe::TriangleDemoFactory());
        m_demoFactories.push_back(new blithe::CubeDemoFactory());
        m_demoFactories.push_back(new blithe::InstancedCubeDemoFactory());
        m_demoFactories.push_back(new blithe::GrassDemoFactory());
        m_demoFactories.push_back(new blithe::SimpleBSPDemoFactory());
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
