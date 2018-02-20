#pragma once

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <CGAL/Polyhedron_incremental_builder_3.h>
#include "Polyhedron.h"

using namespace std;

// A modifier creating a triangle with the incremental builder.
template <class HDS>
class ObjBuilder : public CGAL::Modifier_base<HDS> {
public:
	ObjBuilder(vector<float> verts, vector<int> faces) {
		m_verts = verts;
		m_faces = faces;
	}

	void operator()(HDS& hds) {
		// Postcondition: hds is a valid polyhedral surface.
		CGAL::Polyhedron_incremental_builder_3<HDS> B(hds, true);
		B.begin_surface(m_verts.size()/3, m_faces.size()/3);
		typedef typename HDS::Vertex   Vertex;
		typedef typename Vertex::Point Point;
		for (int i = 0; i < m_verts.size(); i = i + 3)
			B.add_vertex(Point(m_verts[i + 0], m_verts[i + 1], m_verts[i + 2]));
		for (int i = 0; i < m_faces.size(); i = i + 3)
		{
			B.begin_facet();
			//cout << m_faces[i + 0]<<" " << m_faces[i + 1] << " " << m_faces[i + 2] << endl;
			B.add_vertex_to_facet(m_faces[i + 0]);
			B.add_vertex_to_facet(m_faces[i + 1]);
			B.add_vertex_to_facet(m_faces[i + 2]);
			B.end_facet();
		}
		B.end_surface();
	}

private:
	vector<float> m_verts; // every 3 items are a vertex
	vector<int> m_faces; // every 3 items are a face

};
template <class	kernel, class items>
class ObjReader
{
private:
	typedef CGAL::Simple_cartesian<double>     Kernel;
	typedef Enriched_polyhedron<Kernel, Enriched_items>         Polyhedron;
	typedef Polyhedron::HalfedgeDS             HalfedgeDS;

public:
	ObjReader(string fn)
	{
		m_filename = fn;
	}

	void read()
	{
		fstream file;
		file.open(m_filename.c_str(), ios::in);
		readData(file);
		file.close();
	}

	vector<float>& vertices() { return m_vertices; }
	vector<int>& facets() { return m_facets; }

private:
	void readData(fstream& file)
	{
		string str;
		while (getline(file, str))
		{
			stringstream ss(str);

			string tag;
			ss >> tag;

			if (tag == "v")
				parse_vertex(ss);

			if (tag == "f")
				parse_facet(ss);
		}
	}

	void parse_vertex(stringstream& ss)
	{
		float x, y, z;
		ss >> x >> y >> z;
		m_vertices.push_back(x);
		m_vertices.push_back(y);
		m_vertices.push_back(z);
	}

	void parse_facet(stringstream& ss)
	{
		int v1, v2, v3;
		ss >> v1 >> v2 >> v3;
		m_facets.push_back(v1 - 1);
		m_facets.push_back(v2 - 1);
		m_facets.push_back(v3 - 1);
	}

private:
	string m_filename;

	vector<float> m_vertices;
	vector<int> m_facets;
};



//template <class HDS>
//class Builder_obj : public CGAL::Modifier_base<HDS>
//{
//private:
//	typedef typename HDS::Vertex Vertex;
//	typedef typename Vertex::Point Point;
//	typedef typename CGAL::Polyhedron_incremental_builder_3<HDS> Builder;
//	FILE *m_pFile;
//
//public:
//	Builder_obj(FILE *pFile)
//	{
//		m_pFile = pFile;
//	}
//	~Builder_obj() {}
//
//	void operator()(HDS& hds)
//	{
//		Builder builder(hds, true);
//		builder.begin_surface(3, 1, 6);
//		cout << "read vertices"<< endl;
//
//		builder.add_vertex(Point(0, 0, 0));
//
//
//		//read_vertices(builder);
//		cout << "read faces"<< endl;
//		//read_facets(builder);
//		builder.end_surface();
//	}
//
//private:
//	// read vertex coordinates
//	void read_vertices(Builder &builder)
//	{
//		fseek(m_pFile, 0, SEEK_SET);
//		char pLine[512];
//		while (fgets(pLine, 512, m_pFile))
//			if (pLine[0] == 'v')
//			{
//				float x, y, z;
//				if (sscanf(pLine, "v %f %f %f", &x, &y, &z) == 3)
//				{
//					cout << x << " " << y << " " << z << endl;
//					builder.add_vertex(Point(x, y, z));
//				}
//			}
//	}
//
//	// read facets and uv coordinates per halfedge
//	void read_facets(Builder &builder)
//	{
//		fseek(m_pFile, 0, SEEK_SET);
//		char pLine[512];
//		while (fgets(pLine, 512, m_pFile))
//		{
//			char *pTmp = pLine;
//			if (pTmp[0] == 'f')
//			{
//				int index, n;
//				char index_ascii[512], n_ascii[512];
//
//				// create facet        
//				builder.begin_facet();
//
//				pTmp += 2; // jump after 'f '
//				if (strstr(pTmp, "//"))
//					while (sscanf(pTmp, "%d//%d", &index, &n))
//					{
//						itoa(index, index_ascii, 10);
//						itoa(n, n_ascii, 10);
//						builder.add_vertex_to_facet(index - 1);
//						pTmp += (2 + strlen(index_ascii) + strlen(n_ascii));
//						if (strlen(pTmp) < 3)
//							break;
//						else
//							pTmp += 1;
//					}
//				else
//					while (sscanf(pTmp, "%d", &index))
//					{
//						itoa(index, index_ascii, 10);
//						pTmp += strlen(index_ascii);
//						builder.add_vertex_to_facet(index - 1);
//						if (strlen(pTmp) < 3)
//							break;
//						else
//							pTmp += 1;
//					}
//				builder.end_facet();
//			}
//		}
//	}
//};
//
//template <class	kernel, class	items>
//class Parser_obj
//{
//public:
//	typedef typename Enriched_polyhedron<kernel, items>::HalfedgeDS HalfedgeDS;
//	Parser_obj() {}
//	~Parser_obj() {}
//
//public:
//	bool read(const char*pFilename,
//		Enriched_polyhedron<kernel, items> *pMesh)
//	{
//		CGAL_assertion(pMesh != NULL);
//		FILE *pFile = fopen(pFilename, "rt");
//		if (pFile == NULL)
//			return false;
//		Builder_obj<HalfedgeDS> builder(pFile);
//		pMesh->delegate(builder);
//		fclose(pFile);
//		return true;
//	}
//
//};