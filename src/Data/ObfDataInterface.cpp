#include "ObfDataInterface.h"
#include "ObfDataInterface_P.h"

#include "ObfReader.h"
#include "ObfInfo.h"
#include "ObfMapSectionReader.h"
#include "IQueryController.h"

OsmAnd::ObfDataInterface::ObfDataInterface( const QList< std::shared_ptr<ObfReader> >& readers )
    : _d(new ObfDataInterface_P(this, readers))
{
}

OsmAnd::ObfDataInterface::~ObfDataInterface()
{
}

void OsmAnd::ObfDataInterface::obtainMapObjects( QList< std::shared_ptr<const OsmAnd::Model::MapObject> >* resultOut, const AreaI& area31, const ZoomLevel& zoom, IQueryController* controller /*= nullptr*/ )
{
    // Iterate through all OBF readers
    for(auto itObfReader = _d->readers.begin(); itObfReader != _d->readers.end(); ++itObfReader)
    {
        // Check if request is aborted
        if(controller && controller->isAborted())
            return;

        // Iterate over all map sections of each OBF reader
        const auto& obfReader = *itObfReader;
        const auto& obfInfo = obfReader->obtainInfo();
        for(auto itMapSection = obfInfo->mapSections.begin(); itMapSection != obfInfo->mapSections.end(); ++itMapSection)
        {
            // Check if request is aborted
            if(controller && controller->isAborted())
                return;

            // Read objects from each map section
            const auto& mapSection = *itMapSection;
            OsmAnd::ObfMapSectionReader::loadMapObjects(obfReader, mapSection, zoom, &area31, resultOut, nullptr, controller);
        }
    }
}
