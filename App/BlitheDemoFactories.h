#ifndef BLITHEDEMOFACTORIES_H
#define BLITHEDEMOFACTORIES_H

#include <vector>
#include "DemoInterface.h"

namespace blithe
{
    ///
    /// \brief Creates and owns all the demo factories
    ///
    class BlitheDemoFactories
    {
    public:
        BlitheDemoFactories();
        ~BlitheDemoFactories();

        std::vector<DemoFactory*> m_demoFactories; //!< All the Demo Factories
    };
}

#endif // BLITHEDEMOFACTORIES_H
