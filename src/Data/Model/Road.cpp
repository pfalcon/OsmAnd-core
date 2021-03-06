#include "Road.h"

#include <assert.h>

#include "ObfRoutingSectionInfo.h"
#include "ObfRoutingSectionInfo_P.h"
#include "Utilities.h"

OsmAnd::Model::Road::Road(const std::shared_ptr<const ObfRoutingSubsectionInfo>& subsection)
    : subsection(subsection)
    , id(_id)
    , names(_names)
    , points(_points)
    , types(_types)
    , pointsTypes(_pointsTypes)
    , restrictions(_restrictions)
{
}

OsmAnd::Model::Road::Road( const std::shared_ptr<const Road>& that, int insertIdx, uint32_t x31, uint32_t y31 )
    : _ref(that)
    , _id(0)
    , _points(_ref->_points.size() + 1)
    , subsection(_ref->subsection)
    , id(_ref->_id)
    , names(_ref->_names)
    , points(_points)
    , types(_ref->_types)
    , pointsTypes(_pointsTypes)
    , restrictions(_ref->_restrictions)
{
    int pointIdx = 0;
    for(; pointIdx < insertIdx; pointIdx++)
    {
        _points[pointIdx] = _ref->_points[pointIdx];
        if(!_ref->_pointsTypes.isEmpty())
            _pointsTypes.insert(pointIdx, _ref->_pointsTypes[pointIdx]);
    }
    _points[pointIdx] = PointI(x31, y31);
    for(pointIdx += 1; pointIdx < _points.size(); pointIdx++)
    {
        _points[pointIdx] = _ref->_points[pointIdx - 1];
        if(!_ref->_pointsTypes.isEmpty())
            _pointsTypes.insert(pointIdx, _ref->_pointsTypes[pointIdx]);
    }
}

OsmAnd::Model::Road::~Road()
{
}

double OsmAnd::Model::Road::getDirectionDelta( uint32_t originIdx, bool forward ) const
{
    //NOTE: Victor: the problem to put more than 5 meters that BinaryRoutePlanner will treat
    // 2 consequent Turn Right as UT and here 2 points will have same turn angle
    // So it should be fix in both places
    return getDirectionDelta(originIdx, forward, 5);
}

double OsmAnd::Model::Road::getDirectionDelta( uint32_t originIdx, bool forward, float distance ) const
{
    auto itPoint = (_points.begin() + originIdx);
    const auto itOriginPoint = itPoint;
    float scannedDistance = 0.0;
    do
    {
        if(forward)
        {
            itPoint++;
            if(itPoint == _points.end())
            {
                itPoint--;
                break;
            }
        }
        else
        {
            if(itPoint == _points.begin())
                break;
            itPoint--;
        }

        // translate into meters
        scannedDistance +=
            Utilities::x31toMeters(qAbs((int64_t)itPoint->x - (int64_t)itOriginPoint->x)) +
            Utilities::y31toMeters(qAbs((int64_t)itPoint->y - (int64_t)itOriginPoint->y));
    } while ( scannedDistance < distance );

    return -qAtan2(itOriginPoint->x - itPoint->x, itOriginPoint->y - itPoint->y);
}

OsmAnd::Model::RoadDirection OsmAnd::Model::Road::getDirection() const
{
    for(auto itType = types.begin(); itType != types.end(); ++itType)
    {
        const auto& rule = subsection->section->_d->_encodingRules[*itType];

        if(rule->_type == ObfRoutingSectionInfo_P::EncodingRule::OneWay)
            return static_cast<Model::RoadDirection>(rule->_parsedValue.asSignedInt);
        else if(rule->_type == ObfRoutingSectionInfo_P::EncodingRule::Roundabout)
            return Model::RoadDirection::OneWayForward;
    }
    
    return Model::RoadDirection::TwoWay;
}

bool OsmAnd::Model::Road::isRoundabout() const
{
    for(auto itType = types.begin(); itType != types.end(); ++itType)
    {
        const auto& rule = subsection->section->_d->_encodingRules[*itType];

        if(rule->isRoundabout())
            return true;
        else if(rule->getDirection() != Model::RoadDirection::TwoWay && isLoop())
            return true;
    }

    return false;
}

int OsmAnd::Model::Road::getLanes() const
{
    for(auto itType = types.begin(); itType != types.end(); ++itType)
    {
        const auto& rule = subsection->section->_d->_encodingRules[*itType];
        if(rule->_type == ObfRoutingSectionInfo_P::EncodingRule::Lanes)
            return rule->_parsedValue.asUnsignedInt;
    }
    return -1;
}

QString OsmAnd::Model::Road::getHighway() const
{
    for(auto itType = types.begin(); itType != types.end(); ++itType)
    {
        const auto& rule = subsection->section->_d->_encodingRules[*itType];
        if(rule->_type == ObfRoutingSectionInfo_P::EncodingRule::Highway)
            return rule->_value;
    }
    return QString();
}

bool OsmAnd::Model::Road::isLoop() const
{
    assert(_points.size() > 0);

    return _points.first() == _points.last();
}
