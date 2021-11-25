# DijkstraWarehousePlanner
A simple path planner between nodes on a grid (representing a warehouse/ factory) using Dijkstra. Includes waiting times at nodes and travel times.
Open the solution file "CPfinal.sln" with Visual Studio.

The robot moves through a grid from a start waypoint to an end waypoint with waypoints in between (waypoints are also refered to as nodes). The robot traverses these waypoints in order (0,1,2....k). The Dijkstra algorithm is used to decide whether it is faster to skip a waypoint or not. There are 2 time values used for the decision:
1) The time taken to travel between nodes (the movespeed of the robot is constant)
2) the constant waiting time assigned to each node (currently 10s, can be changed)

The following values can be changed (at the beginning of the main() function):
1) the grid size
2) start and end positiong
3) the robot speed (determines the travel time between nodes)
4) the wait time at each node

Here is an image outlining the problem:
![Alt text](/visualrepresentation.PNG?raw=true "Visual Representation of Problem")

Here is some sample input:
3
30 30 90
60 60 80
10 90 100

which means:
3 nodes
node 1: x = 30, y = 30, wait time at node = 90
node 2: x = 60, y = 60, wait time at node = 80
node 3: x = 10, y = 90, wait time at node = 100

and the sample output:
156.858

which means:
156.858 seconds to travel from the initial position to the final position
