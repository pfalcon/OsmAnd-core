#include "ObfMapSectionInfo_P.h"

OsmAnd::ObfMapSectionInfo_P::ObfMapSectionInfo_P( ObfMapSectionInfo* owner_ )
    : owner(owner_)
{
}

OsmAnd::ObfMapSectionInfo_P::~ObfMapSectionInfo_P()
{
}

OsmAnd::ObfMapSectionInfo_P::Rules::Rules()
    : _nameEncodingType(0)
    , _refEncodingType(-1)
    , _coastlineEncodingType(-1)
    , _coastlineBrokenEncodingType(-1)
    , _landEncodingType(-1)
    , _onewayAttribute(-1)
    , _onewayReverseAttribute(-1)
{
    _positiveLayers.reserve(2);
    _negativeLayers.reserve(2);
}
