# CBusSystemIndexer

## Purpose
Builds an index of bus stops and bus routes provided through the CBusSystem interface. It allows the other components of trip planning to efficiently access stop information, find stops by ID number, and determine which routes serve per stop.

## Format
takes a shared pointer to a CBusSystem object. when the object is created it reads all stops and routes from the bus system and builds lookup structures.

## Indexing
- stores all stops in a vector
- creates mapping from stop ID to index
- creates mapping from stop index to routes

## Role
core classes in the project, converts raw bus system data into a form that supports our other files to plan routes and lookup stops. used by CTripPlanner.