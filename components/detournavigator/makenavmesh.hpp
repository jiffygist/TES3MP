#ifndef OPENMW_COMPONENTS_DETOURNAVIGATOR_MAKENAVMESH_H
#define OPENMW_COMPONENTS_DETOURNAVIGATOR_MAKENAVMESH_H

#include "offmeshconnectionsmanager.hpp"
#include "settings.hpp"
#include "navmeshcacheitem.hpp"
#include "tileposition.hpp"
#include "tilebounds.hpp"
#include "sharednavmesh.hpp"
#include "navmeshtilescache.hpp"

#include <osg/Vec3f>

#include <memory>

class dtNavMesh;

namespace DetourNavigator
{
    class RecastMesh;
    struct Settings;

    enum class UpdateNavMeshStatus : unsigned
    {
        ignored = 0,
        removed = 1 << 0,
        added = 1 << 1,
        replaced = removed | added,
        failed = 1 << 2,
        lost = removed | failed,
    };

    inline bool isSuccess(UpdateNavMeshStatus value)
    {
        return (static_cast<unsigned>(value) & static_cast<unsigned>(UpdateNavMeshStatus::failed)) == 0;
    }

    inline float getLength(const osg::Vec2i& value)
    {
        return std::sqrt(float(osg::square(value.x()) + osg::square(value.y())));
    }

    inline float getDistance(const TilePosition& lhs, const TilePosition& rhs)
    {
        return getLength(lhs - rhs);
    }

    inline bool shouldAddTile(const TilePosition& changedTile, const TilePosition& playerTile, int maxTiles)
    {
        const auto expectedTilesCount = std::ceil(osg::PI * osg::square(getDistance(changedTile, playerTile)));
        return expectedTilesCount <= maxTiles;
    }

    NavMeshPtr makeEmptyNavMesh(const Settings& settings);

    UpdateNavMeshStatus updateNavMesh(const osg::Vec3f& agentHalfExtents, const RecastMesh* recastMesh,
        const TilePosition& changedTile, const TilePosition& playerTile,
        const std::vector<OffMeshConnection>& offMeshConnections, const Settings& settings,
        const SharedNavMeshCacheItem& navMeshCacheItem, NavMeshTilesCache& navMeshTilesCache);
}

#endif
