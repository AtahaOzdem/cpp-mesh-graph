# 3D Mesh Graph & Processing

A C++ implementation for parsing 3D triangular meshes in **OFF (Object File Format)** and performing graph-based queries and geometric calculations on them.

3D meshes are represented as undirected graphs consisting of vertices, edges, and triangular faces. This project builds adjacency structures directly from `.off` files and implements common traversal and heap operations.

## Features

- **OFF File Parser**: Reads 3D vertex coordinates and triangle definitions, automatically constructing unique edges and adjacency lists.
- **Floyd's Min-Heap (`buildHeap`)**: Orders mesh edges by Euclidean length in $O(E)$ time and supports extracting the $k$-th shortest edge (`getKthShortestEdge`).
- **Unweighted Shortest Path (`getJumpCount`)**: Uses Breadth-First Search (BFS) to find the minimum number of edge hops between two vertices.
- **Geodesic Distance (`getGeodesicDistance`)**: Uses Dijkstra's algorithm with a min-priority queue to compute the shortest surface path distance between two vertices.
- **Dynamic Mesh Updates (`updateVertex`)**: Updates vertex coordinates in 3D space, recalculates incident edge lengths, and rebuilds the heap.

## Project Structure

- `DataStructures.h` — Core data structures (`Vertex`, `Edge`, `Triangle`).
- `Arithmetic.h` — Helper functions for 3D vector math and Euclidean distance.
- `Mesh.h` / `Mesh.cpp` — Mesh parser, heap operations, BFS, and Dijkstra implementations.
- `main.cpp` — Demo program showcasing the algorithms on sample meshes.
- `models/` — Sample `.off` 3D models (`bunny5.off`, `cube.off`, `spot.off`).

## Build & Run

Compile using `g++` (C++11 or later):

```bash
g++ main.cpp Mesh.cpp -o mesh_demo -std=c++11
```

Run the default showcase:

```bash
./mesh_demo
```

Or run the analysis on a specific `.off` file:

```bash
./mesh_demo models/spot.off
```
