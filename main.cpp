#include <iostream>
#include <iomanip>
#include <vector>
#include <string>
#include "Mesh.h"

void printSeparator(char c = '-', int width = 76) {
	std::cout << std::string(width, c) << "\n";
}

void printModelSummary(const std::string& label, const std::string& path) {
	Mesh mesh(path.c_str());
	size_t V = mesh.verts.size();
	size_t E = mesh.edges.size();
	size_t F = mesh.tris.size();
	int euler = static_cast<int>(V) - static_cast<int>(E) + static_cast<int>(F);

	std::cout << std::left << std::setw(22) << label
	          << std::setw(22) << path
	          << std::right << std::setw(8) << V
	          << std::setw(10) << E
	          << std::setw(10) << F
	          << std::setw(6) << euler << "\n";
}

void runDetailedShowcase(const std::string& filepath) {
	std::cout << "\n";
	printSeparator('=');
	std::cout << "  DETAILED ALGORITHMIC SHOWCASE: " << filepath << "\n";
	printSeparator('=');

	Mesh mesh(filepath.c_str());

	if (mesh.verts.empty()) {
		std::cerr << "[Error] Could not load mesh from: " << filepath
		          << " (Make sure you run the executable from the project root directory).\n";
		return;
	}

	// 1. Topology & Local Neighborhood Inspection
	std::cout << "\n[1] Vertex & Topology Inspection (Vertex #0)\n";
	printSeparator();
	const Vertex& v0 = mesh.getVertex(0);
	std::cout << std::fixed << std::setprecision(6);
	std::cout << "  Coordinates        : (" << v0.coords[0] << ", " << v0.coords[1] << ", " << v0.coords[2] << ")\n";
	std::cout << "  Vertex Degree      : " << mesh.getDegree(0) << " connected edges/neighbors\n";
	std::cout << "  Adjacent Triangles : " << v0.triList.size() << " surfaces\n";
	if (!v0.vertList.empty()) {
		size_t firstNeighbor = v0.vertList[0];
		std::cout << "  First Neighbor ID  : Vertex #" << firstNeighbor
		          << " (isVertsNeighbour(0, " << firstNeighbor << ") = "
		          << (mesh.isVertsNeighbour(0, firstNeighbor) ? "true" : "false") << ")\n";
	}

	// 2. Floyd's Min-Heap & K-th Shortest Edge Extraction
	std::cout << "\n[2] Floyd's Min-Heap: Top 5 Shortest Edges (getKthShortestEdge)\n";
	printSeparator();
	std::cout << std::left << std::setw(10) << "  Rank"
	          << std::setw(14) << "Edge Index"
	          << std::setw(24) << "Vertices (v1 <-> v2)"
	          << std::right << std::setw(16) << "Length\n";
	printSeparator();

	size_t maxRank = std::min<size_t>(5, mesh.edges.size());
	for (size_t k = 0; k < maxRank; ++k) {
		const Edge& e = mesh.getKthShortestEdge(k);
		std::string pairStr = "#" + std::to_string(e.v1i) + " <-> #" + std::to_string(e.v2i);
		std::cout << "  #" << std::left << std::setw(7) << (k + 1)
		          << std::setw(14) << e.idx
		          << std::setw(24) << pairStr
		          << std::right << std::setw(16) << std::setprecision(6) << e.length << "\n";
	}

	// 3. Shortest Path Comparison: BFS Jump Count vs. Dijkstra Geodesic Distance
	std::cout << "\n[3] Pathfinding: BFS Hop Count vs. Dijkstra Geodesic Distance\n";
	printSeparator();
	std::cout << std::left << std::setw(18) << "  Vertex Pair"
	          << std::right << std::setw(12) << "BFS Hops"
	          << std::setw(18) << "Euclidean Dist"
	          << std::setw(18) << "Geodesic Dist"
	          << std::setw(10) << "Ratio\n";
	printSeparator();

	size_t nV = mesh.verts.size();
	std::vector<std::pair<size_t, size_t>> testPairs;
	if (!v0.vertList.empty()) {
		testPairs.push_back({0, v0.vertList[0]}); // Direct neighbor (1 hop)
	}
	testPairs.push_back({0, nV / 4});
	testPairs.push_back({0, nV / 2});
	testPairs.push_back({nV / 3, (2 * nV) / 3});
	testPairs.push_back({0, nV - 1});

	for (size_t i = 0; i < testPairs.size(); ++i) {
		size_t u = testPairs[i].first;
		size_t v = testPairs[i].second;

		int hops = mesh.getJumpCount(u, v);
		double euclidean = dist(mesh.getVertex(u).coords, mesh.getVertex(v).coords);
		double geodesic = mesh.getGeodesicDistance(u, v);
		double ratio = (euclidean > 1e-9) ? (geodesic / euclidean) : 1.0;

		std::string label = "  #" + std::to_string(u) + " -> #" + std::to_string(v);
		std::cout << std::left << std::setw(18) << label
		          << std::right << std::setw(12) << hops
		          << std::setw(18) << std::setprecision(5) << euclidean
		          << std::setw(18) << std::setprecision(5) << geodesic
		          << std::setw(9) << std::setprecision(2) << ratio << "x\n";
	}

	// 4. Dynamic Vertex Update & Heap Re-ordering
	std::cout << "\n[4] Dynamic Mesh Update & Heap Reconstruction (updateVertex)\n";
	printSeparator();
	const Edge& shortestBefore = mesh.getKthShortestEdge(0);
	size_t targetVert = shortestBefore.v1i;
	double oldX = mesh.getVertex(targetVert).coords[0];
	double oldY = mesh.getVertex(targetVert).coords[1];
	double oldZ = mesh.getVertex(targetVert).coords[2];

	std::cout << std::setprecision(6);
	std::cout << "  Shortest Edge Before Update : Edge #" << shortestBefore.idx
	          << " (Vertices #" << shortestBefore.v1i << " <-> #" << shortestBefore.v2i
	          << "), Length = " << shortestBefore.length << "\n";
	std::cout << "  Updating Vertex #" << targetVert << " position : ("
	          << oldX << ", " << oldY << ", " << oldZ << ") -> ("
	          << oldX + 0.5 << ", " << oldY + 0.5 << ", " << oldZ + 0.5 << ")\n";

	mesh.updateVertex(targetVert, oldX + 0.5, oldY + 0.5, oldZ + 0.5);

	const Edge& shortestAfter = mesh.getKthShortestEdge(0);
	std::cout << "  Shortest Edge After Update  : Edge #" << shortestAfter.idx
	          << " (Vertices #" << shortestAfter.v1i << " <-> #" << shortestAfter.v2i
	          << "), Length = " << shortestAfter.length << "\n";
	printSeparator('=');
}

int main(int argc, char* argv[]) {
	std::cout << "\n";
	printSeparator('=');
	std::cout << "  3D TRIANGULAR MESH PROCESSING & GRAPH ALGORITHMS\n";
	printSeparator('=');

	if (argc > 1) {
		runDetailedShowcase(argv[1]);
		return 0;
	}

	// Overview of all sample 3D OFF models in the repository
	std::cout << "\n[Overview] Sample 3D Watertight Manifold Meshes (Euler: V - E + F = 2)\n";
	printSeparator();
	std::cout << std::left << std::setw(22) << "Model Name"
	          << std::setw(22) << "File Path"
	          << std::right << std::setw(8) << "Verts"
	          << std::setw(10) << "Edges"
	          << std::setw(10) << "Tris"
	          << std::setw(6) << "Euler" << "\n";
	printSeparator();

	printModelSummary("Stanford Bunny", "models/bunny5.off");
	printModelSummary("Subdivided Cube", "models/cube.off");
	printModelSummary("Spot the Cow", "models/spot.off");
	printSeparator();

	// Run full algorithmic demonstration on Stanford Bunny
	runDetailedShowcase("models/bunny5.off");

	return 0;
}
