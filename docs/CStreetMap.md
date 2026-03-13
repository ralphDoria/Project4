# CStreetMap Interface

## Purpose
this map is an abstract interface that describes how a street map must behave

street map is made up of a Node and a Way:
- Node: points with a latitude/longitude
- Ways: ordered lists of node IDs that represent a path

### Node
- object with: id, ordered list of node ids, optional attributes

### Way
- object with: id, ordered list of node ids, optional attributes