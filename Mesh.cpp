#include "Mesh.h"

Mesh::Mesh(const char* filename) {
	int nVerts, nTris, n, k = 0;
	double x, y, z;
	minHeap.push_back(0);

	ifstream file(filename);

	string line;
	getline(file, line);
	getline(file, line);
	stringstream str(line);
	str >> nVerts >> nTris >> n;

	while (k++ < nVerts) {
		getline(file, line);
		stringstream str2(line);

		str2 >> x >> y >> z;
		addVertex(x, y, z);
	}

	k = 0;
	while (k++ < nTris) {
		getline(file, line);
		stringstream str2(line);

		str2 >> n >> x >> y >> z;
		addTriangle((size_t)x, (size_t)y, (size_t)z);
	}

	file.close();

	buildHeap();
}


size_t Mesh::addVertex(double x, double y, double z) {
	Vertex new_vert(verts.size(), x, y, z);
	verts.push_back(new_vert);
	return new_vert.idx;
}

size_t Mesh::addTriangle(size_t v1, size_t v2, size_t v3) {
	Triangle new_tri(tris.size(), v1, v2, v3);
	tris.push_back(new_tri);

	verts[v1].triList.push_back(new_tri.idx);
	verts[v2].triList.push_back(new_tri.idx);
	verts[v3].triList.push_back(new_tri.idx);
	
	bool is_v1_v2 = false;
	if (!edges.empty()) {
		Edge& e = getEdge(v1, v2);
		if ((e.v1i == v1 && e.v2i == v2) || (e.v1i == v2 && e.v2i == v1)) {
			is_v1_v2 = true;
		}
	}

	if (!is_v1_v2) {
		Edge new_edge(edges.size(), v1, v2);
		new_edge.length = dist(verts[v1].coords, verts[v2].coords);
		edges.push_back(new_edge);

		verts[v1].edgeList.push_back(new_edge.idx);
		verts[v2].edgeList.push_back(new_edge.idx);

		verts[v1].vertList.push_back(v2);
		verts[v2].vertList.push_back(v1);
	}

	bool is_v2_v3 = false;
	if (!edges.empty()) {
		Edge& e = getEdge(v2, v3);
		if ((e.v1i == v2 && e.v2i == v3) || (e.v1i == v3 && e.v2i == v2)) {
			is_v2_v3 = true;
		}
	}

	if (!is_v2_v3) {
		Edge new_edge(edges.size(), v2, v3);
		new_edge.length = dist(verts[v2].coords, verts[v3].coords);
		edges.push_back(new_edge);
		verts[v2].edgeList.push_back(new_edge.idx);
		verts[v3].edgeList.push_back(new_edge.idx);
		
		verts[v2].vertList.push_back(v3);
		verts[v3].vertList.push_back(v2);
	}

	bool is_v1_v3 = false;
	if (!edges.empty()) {
		Edge& e = getEdge(v1, v3);
		if ((e.v1i == v1 && e.v2i == v3) || (e.v1i == v3 && e.v2i == v1)) {
			is_v1_v3 = true;
		}
	}
	
	if (!is_v1_v3) {
		Edge new_edge(edges.size(), v1, v3);
		new_edge.length = dist(verts[v1].coords, verts[v3].coords);
		edges.push_back(new_edge);
		verts[v1].edgeList.push_back(new_edge.idx);
		verts[v3].edgeList.push_back(new_edge.idx);

		verts[v1].vertList.push_back(v3);
		verts[v3].vertList.push_back(v1);
	}

	return new_tri.idx;
}

void Mesh::buildHeap() {
	minHeap.clear();
	minHeap.push_back(0);
	for (size_t i=0; i < edges.size(); i++) {
		minHeap.push_back(i);
	}

	for (size_t i = (minHeap.size() - 1) / 2; i > 0; i--) {
		size_t parent = i;

		while (parent * 2 <= minHeap.size() - 1) {
			size_t child = parent * 2;

			if ((child != minHeap.size() - 1) && (edges[minHeap[child + 1]].length < edges[minHeap[child]].length)) {
				child++;
			}
			double parent_len = edges[minHeap[parent]].length;
			double child_len = edges[minHeap[child]].length;
			
			if (child_len < parent_len) {
				size_t temp = minHeap[parent];
				minHeap[parent] = minHeap[child];
				minHeap[child] = temp;

				parent = child;
			} else {
				break;
			}
		}
	}
}

Edge& Mesh::getEdge(size_t v1, size_t v2) {
	if (v1 >= verts.size() || v2 >= verts.size()) 
		return edges[0];

	for (size_t i=0; i < verts[v1].edgeList.size(); i++) {
		size_t edge = verts[v1].edgeList[i];
		if (edges[edge].v1i == v2 || edges[edge].v2i == v2) 
			return edges[edge];
	}
	return edges[0];
}

void Mesh::printVertex(size_t v) {
	if (v < verts.size()) {
		std::cout << verts[v].coords[0] << " " << verts[v].coords[1] << " " << verts[v].coords[2] << " ";
	}
}

void Mesh::printTriangle(size_t t) {
	if (t < tris.size()) {
		std::cout << "3 " << tris[t].v1i << " " << tris[t].v2i << " " << tris[t].v3i << " ";
	}
}

bool Mesh::isVertsNeighbour(size_t v1, size_t v2) {
	if (v1 >= verts.size() || v2 >= verts.size())
		return false;
	if (edges.empty())
		return false;

	size_t v1_v2 = getEdge(v1, v2).idx;
	if ((edges[v1_v2].v1i == v1 && edges[v1_v2].v2i == v2) || (edges[v1_v2].v1i == v2 && edges[v1_v2].v2i == v1))
		return true;
	else	
		return false;
}

size_t Mesh::getDegree(size_t v) {
	if (v >= verts.size())
		return 0;
	return verts[v].vertList.size();
}

int Mesh::getJumpCount(size_t v1, size_t v2) {
	if (v1 >= verts.size() || v2 >= verts.size())
		return -1;

	std::queue<size_t> q;
	std::vector<int> distances(verts.size(), -1);

	q.push(v1);
	distances[v1] = 0;

	while (!q.empty()) {
		size_t curr = q.front();
		q.pop();
		if (curr == v2) 
			return distances[curr];

		for (size_t i=0; i < verts[curr].vertList.size(); i++) {
			size_t neighbor = verts[curr].vertList[i];
			if (distances[neighbor] == -1) {
				distances[neighbor] = distances[curr] + 1;
				q.push(neighbor);
			}
		}
	}
	return -1;
}

double Mesh::getGeodesicDistance(size_t v1, size_t v2) {
	if (v1 >= verts.size() || v2 >= verts.size())
		return NONE;
	if (v1 == v2)
		return 0.0;
	
	std::vector<double> distances(verts.size(), NONE);
	std::vector<bool> visited(verts.size(), false);

	typedef std::pair<double, size_t> DistVertex;
	std::priority_queue<DistVertex, std::vector<DistVertex>, std::greater<DistVertex>> pq;

	distances[v1] = 0.0;
	pq.push({0.0, v1});

	while (!pq.empty()) {
		size_t curr = pq.top().second;
		pq.pop();

		if (visited[curr])
			continue;			
		visited[curr] = true;
		
		if (curr == v2)
			return distances[curr];

		for (size_t i=0; i < verts[curr].edgeList.size(); i++) {
			Edge& e = edges[verts[curr].edgeList[i]];
			size_t neighbor = (e.v1i == curr) ? e.v2i : e.v1i;

			if (!visited[neighbor]) {
				double new_dist = distances[curr] + e.length;
				if (new_dist < distances[neighbor]) {
					distances[neighbor] = new_dist;
					pq.push({new_dist, neighbor});
				}
			}
		}
	}

	return NONE;
}

void Mesh::updateVertex(size_t v, double x, double y, double z) {
	if (v >= verts.size())
		return;

	assign(verts[v].coords, x, y, z);

	for (size_t i=0; i < verts[v].edgeList.size(); i++) {
		size_t edge_ind = verts[v].edgeList[i];
		Edge& e = edges[edge_ind];
		double new_len = dist(verts[e.v1i].coords, verts[e.v2i].coords);

		e.length = new_len;
	}
	
	buildHeap();
}

Edge& Mesh::getKthShortestEdge(size_t k) {
	if (k >= edges.size())
		return edges[0];
	
	std::vector<size_t> tempHeap = minHeap;

	for (size_t i=0; i < k; i++) {
		tempHeap[1] = tempHeap.back();
		tempHeap.pop_back();

		size_t parent = 1;
		while (parent * 2 <= tempHeap.size() - 1) {
			size_t child = parent * 2;
			if ((child != tempHeap.size() - 1) && (edges[tempHeap[child + 1]].length < edges[tempHeap[child]].length)) {
				child++;
			}
			double parent_len = edges[tempHeap[parent]].length;
			double child_len = edges[tempHeap[child]].length;

			if (child_len < parent_len) {
				size_t temp = tempHeap[parent];
				tempHeap[parent] = tempHeap[child];
				tempHeap[child] = temp;

				parent = child;
			} else {
				break;
			}
		}
	}

	return edges[tempHeap[1]];
}
