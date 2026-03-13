# CTripPlanner

## Overview
Finds us routes or travel plans based given a start/destination stop sign ID, and other potential parameters, such as a time to leave at or a time to arrive by. It will either find a direct route, or a travel plan with at most one bus transfer.

Defines:
STravelStep - contains data members such as stop id, stop time, and name of route
TTravelPlan - A vector of travel steps