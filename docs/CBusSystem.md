# CBusSystem Interface

## Purpose
interface representing a bus system composed of stops, routes, and paths

### Stop
a stop represents a location where buses can stop
- has a stop id
- has a node id
- has a description

### Route
a route represents a sequiences of node ids between stop node endpoints
- includes start node and end node
- contains ordered node ids for the specific route