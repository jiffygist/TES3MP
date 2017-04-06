#ifndef OPENMW_CELL_HPP
#define OPENMW_CELL_HPP

#include "LocalActor.hpp"
#include "DedicatedActor.hpp"
#include "../mwworld/cellstore.hpp"

namespace mwmp
{
    class Cell
    {
    public:

        Cell(MWWorld::CellStore* cellStore);
        ~Cell();

        void update();
        void initializeLocalActors();

        std::string generateMapIndex(MWWorld::Ptr ptr);

        MWWorld::CellStore* getCellStore();
        std::string getDescription();

    private:
        MWWorld::CellStore* store;
        std::map<std::string, mwmp::LocalActor *> localActors;
        std::map<std::string, mwmp::DedicatedActor *> dedicatedActors;
    };
}

#endif //OPENMW_CELL_HPP
