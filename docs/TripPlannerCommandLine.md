# CTripPlannerCommandLine

## Purpose 
this class provides the text-based user interface that the interacts with the trip planner. it is essentially just the program for the user to run. it reads commands from a list of commands with the corresponding input sources and then writes out the results to that requested command.

The class acts as a controller for the entirety of the project that connects all the configuration in one usable source.

## Storage
the implementation stores several pieces of data.
* access to the planner, map, sinks, writers, and files needed to process all the commands
* it store the most recent trip plan
* store the file name with the last trip
