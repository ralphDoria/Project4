# COpenStreeMap

## Overview
implementation of CStreetMap that parses an OpenStreeMap styel XML file using CXMLReader
reads nodes, ways, and tag data that are inside of the nodes and way entities

Uses a PIMPL Implementation to hide details from the interface

### Nodes
reads the requires attrs, id, lat ,lon
collects <tag> entries and stores as atrrs
stores the node in the DNodesByIndex and DNodesByID for lookup

### Ways
reads the id
collects the node references and attribute data
stores the way in DWaysByInfex and DWaysByID