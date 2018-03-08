///////////////////////////////////////////////////////////////////////////
//																																			 //
//	Class: Enriched_polyhedron                                           //
//																																			 //
///////////////////////////////////////////////////////////////////////////

#ifndef	_POLYGON_MESH_
#define	_POLYGON_MESH_

// CGAL	
//#include <CGAL/Cartesian.h>
#include <CGAL/Polyhedron_3.h>


// a refined facet with a normal and a tag
template <class	Refs, class	T, class P, class Norm>
class	Enriched_facet : public	CGAL::HalfedgeDS_face_base<Refs, T>
{
private:
	// idx
	int m_idx;

	// tag
	int	m_tag;

	bool m_selected;

	// normal
	Norm m_normal;


public:

	// life	cycle
	// no	constructors to	repeat,	since	only
	// default constructor mandatory

	Enriched_facet()
	{
	}

	// idx
	const int& idx() const { return m_idx; }
	int& idx() { return m_idx; }

	// tag
	const int& tag() const { return m_tag; }
	int& tag() { return m_tag; }

	const bool& selected() const { return m_selected; }
	bool& selected() { return m_selected; }

	// normal
	typedef	Norm Normal_3;
	Normal_3&	normal() { return	m_normal; }
	const	Normal_3&	normal() const { return	m_normal; }
};



// a refined halfedge with a general tag and 
// a binary tag to indicate wether it belongs 
// to the control mesh or not
template <class	Refs, class	Tprev, class Tvertex, class	Tface, class Norm>
class	Enriched_halfedge : public CGAL::HalfedgeDS_halfedge_base<Refs, Tprev, Tvertex, Tface>
{
private:
	// idx
	int m_idx;

	// general-purpose tag
	int	m_tag;

	bool m_selected;

	// option	for	edge superimposing
	bool m_control_edge;

	// cotan weight
	double m_cotan;



public:

	// life	cycle
	Enriched_halfedge()
	{
		m_control_edge = true;
		m_cotan = 0;
	}

	// idx
	const int& idx() const { return m_idx; }
	int& idx() { return m_idx; }
	
	// tag
	const int& tag() const { return m_tag; }
	int& tag() { return m_tag; }

	const bool& selected() const { return m_selected; }
	bool& selected() { return m_selected; }

	// cotan
	const double& cotan() const { return m_cotan; }
	double& cotan() { return m_cotan; }

	// control edge	
	bool& control_edge() { return m_control_edge; }
	const bool& control_edge()	const { return m_control_edge; }

	double length() 
	{
		return sqrt((vertex()->point() - opposite()->vertex()->point()).squared_length());
	}
};



// a refined vertex with a normal and a tag
template <class	Refs, class	T, class P, class N, class	Kernel>
class	Enriched_vertex : public CGAL::HalfedgeDS_vertex_base<Refs, T, P>
{
public:
	typedef	typename N Normal;
	typedef typename Refs::Vertex_handle Vertex_handle;


private:
	// idx
	int m_idx;

	int	m_tag;

	bool m_selected;
	int m_pos;

	Normal m_normal;

public:
	// life	cycle
	Enriched_vertex()
	{
	}
	// repeat	mandatory	constructors
	Enriched_vertex(const	P& pt)
		: CGAL::HalfedgeDS_vertex_base<Refs, T, P>(pt)
	{
	}

	// idx
	const int& idx() const { return m_idx; }
	int& idx() { return m_idx; }

	// normal
	Normal&	normal() { return	m_normal; }
	const	Normal&	normal() const { return	m_normal; }

	// tag
	int& tag() { return m_tag; }
	const int& tag() const { return m_tag; }

	const bool& selected() const { return m_selected; }
	bool& selected() { return m_selected; }

	int& pos() { return m_pos; }
	const int& pos() const { return m_pos; }


};


struct Enriched_items : public CGAL::Polyhedron_items_3
{
	// wrap	vertex
	template <class	Refs, class	Traits>
	struct Vertex_wrapper
	{
		typedef	typename Traits::Point_3 Point;
		typedef	typename Traits::Vector_3	Normal;
		typedef	Enriched_vertex<Refs,
			CGAL::Tag_true,
			Point,
			Normal,
			Traits>	Vertex;
	};

	// wrap	face
	template <class	Refs, class	Traits>
	struct Face_wrapper
	{
		typedef	typename Traits::Point_3	Point;
		typedef	typename Traits::Vector_3	Normal;
		typedef	Enriched_facet<Refs,
			CGAL::Tag_true,
			Point,
			Normal> Face;
	};

	// wrap	halfedge
	template <class	Refs, class	Traits>
	struct Halfedge_wrapper
	{
		typedef	typename Traits::Vector_3	Normal;
		typedef	Enriched_halfedge<Refs,
			CGAL::Tag_true,
			CGAL::Tag_true,
			CGAL::Tag_true,
			Normal>	Halfedge;
	};
};


//*********************************************************
template <class	kernel, class	items>
class	Enriched_polyhedron : public CGAL::Polyhedron_3<kernel, items>
{
public:
	typedef	typename kernel::FT	FT;
	typedef	typename kernel::Point_3 Point;
	typedef	typename kernel::Vector_3	Vector;

private:
	// bounding box
	FT m_min[3];
	FT m_max[3];
	FT m_radius_approx;
	FT m_center[3];

	// type
	bool m_pure_quad;
	bool m_pure_triangle;

	// connected components, genus, #boundaries
	int m_g;
	unsigned int m_c;
	unsigned int m_b;

	// map from index to elements
	std::map<int, Vertex_iterator> idx2Vertex;
	std::map<int, Halfedge_iterator> idx2Halfedge;
	std::map<int, Facet_iterator> idx2Facet;

public:

	unsigned int c() { return m_c; }
	int g() { return m_g; }
	unsigned int b() { return m_b; }

	std::list<Halfedge_handle> m_seed_boundaries;

public:

	// life	cycle
	Enriched_polyhedron()
	{
		m_pure_quad = false;
		m_pure_triangle = false;
	}
	virtual	~Enriched_polyhedron()
	{
	}

	// type
	bool is_pure_triangle() { return m_pure_triangle; }
	bool is_pure_quad() { return m_pure_quad; }


	Vertex_handle random_vertex()
	{
		int nbv = size_of_vertices();
		int r = (int)((double)rand() / (double)RAND_MAX * (double)nbv);
		r = (r >= nbv) ? nbv - 1 : r;
		Vertex_iterator it = vertices_begin();
		for (int i = 0; i < r; i++)
			it++;
		return it;
	}

	Facet_handle random_facet()
	{
		int nbf = size_of_facets();
		int r = (int)((double)rand() / (double)RAND_MAX * (double)nbf);
		r = (r >= nbf) ? nbf - 1 : r;
		Facet_iterator it = facets_begin();
		for (int i = 0; i < r; i++)
			it++;
		return it;
	}


	// check wether	a	vertex is	on a boundary	or not
	static bool	is_border(Vertex_handle	v)
	{
		Halfedge_around_vertex_circulator	he = v->vertex_begin();
		if (he == NULL)	// isolated	vertex
			return true;
		Halfedge_around_vertex_circulator	end = he;
		CGAL_For_all(he, end)
			if (he->is_border())
				return true;
		return false;
	}


	Point centroid(Facet_handle f)
	{
		Halfedge_handle he = f->halfedge();
		const Point& a = he->vertex()->point();
		const Point& b = he->next()->vertex()->point();
		const Point& c = he->next()->next()->vertex()->point();
		return CGAL::centroid(a, b, c);
	}

	FT sqd(const Point& a,
		const Point& b)
	{
		return (a - b) * (a - b);
	}

	FT len(Halfedge_handle he)
	{
		const Point& a = he->vertex()->point();
		const Point& b = he->opposite()->vertex()->point();
		return std::sqrt(sqd(a, b));
	}


	unsigned int nb_incident_facets_tag(Facet_handle f,
		const int tag)
	{
		Halfedge_around_facet_circulator he = f->facet_begin();
		Halfedge_around_facet_circulator end = he;
		unsigned int nb = 0;
		CGAL_For_all(he, end)
		{
			Facet_handle nf = he->opposite()->facet();
			if (nf != NULL && nf->tag() == tag)
				nb++;
		}
		return nb;
	}

	//basic init
	void basic_init()
	{
		index_elements_build_map();
		compute_normals();
		compute_bounding_box();
	}

	// index all elements
	void index_elements_build_map()
	{
		int k;

		k = 0;
		for (Vertex_iterator vi = vertices_begin(); vi != vertices_end(); vi++, ++k) {
			vi->idx() = k;
			vi->selected() = false;
			idx2Vertex[k] = vi;
		}

		k = 0;
		for (Halfedge_iterator hei = halfedges_begin(); hei != halfedges_end(); hei++, ++k) {
			hei->idx() = k;
			hei->selected() = false;
			idx2Halfedge[k] = hei;
		}

		k = 0;
		for (Facet_iterator fi = facets_begin(); fi != facets_end(); fi++, ++k) {
			fi->idx() = k;
			fi->selected() = false;
			idx2Facet[k] = fi;
		}
	}

	Vertex_iterator vertex(int idx)
	{
		return idx2Vertex[idx];
	}

	Halfedge_iterator halfedge(int idx)
	{
		return idx2Halfedge[idx];
	}

	Facet_iterator facet(int idx)
	{
		return idx2Facet[idx];
	}
	
	// normals (per	facet, then	per	vertex)
	void compute_normals_per_facet()
	{
		std::for_each(facets_begin(), facets_end(), Facet_normal());
	}
	void compute_normals_per_vertex()
	{
		std::for_each(vertices_begin(), vertices_end(), Vertex_normal());
	}
	void compute_normals()
	{
		compute_normals_per_facet();
		compute_normals_per_vertex();
	}

	// compute bounding	box
	void compute_bounding_box()
	{
		FT xmin;
		FT xmax;
		FT ymin;
		FT ymax;
		FT zmin;
		FT zmax;

		if (size_of_vertices() == 0)
			return;

		Vertex_iterator	pVertex = vertices_begin();
		xmin = xmax = pVertex->point().x();
		ymin = ymax = pVertex->point().y();
		zmin = zmax = pVertex->point().z();
		for (; pVertex != vertices_end(); pVertex++)
		{
			const Point& p = pVertex->point();
			xmin = std::min(xmin, p.x());
			ymin = std::min(ymin, p.y());
			zmin = std::min(zmin, p.z());
			xmax = std::max(xmax, p.x());
			ymax = std::max(ymax, p.y());
			zmax = std::max(zmax, p.z());
		}
		m_min[0] = xmin; m_max[0] = xmax;
		m_min[1] = ymin; m_max[1] = ymax;
		m_min[2] = zmin; m_max[2] = zmax;

		m_center[0] = (m_min[0] + m_max[0]) / 2;
		m_center[1] = (m_min[1] + m_max[1]) / 2;
		m_center[2] = (m_min[2] + m_max[2]) / 2;

		m_radius_approx = max(m_max[0] - m_min[0], m_max[1] - m_min[1]);
		m_radius_approx = max(m_max[2] - m_min[2], m_radius_approx) / 2;
	}

	// bounding box
	FT xmin() { return m_min[0]; }
	FT xmax() { return m_max[0]; }
	FT ymin() { return m_min[1]; }
	FT ymax() { return m_max[1]; }
	FT zmin() { return m_min[2]; }
	FT zmax() { return m_max[2]; }
	FT xcenter() { return m_center[0]; }
	FT ycenter() { return m_center[1]; }
	FT zcenter() { return m_center[2]; }
	FT radius() { return m_radius_approx; }


	// copy bounding box
	void copy_bounding_box(Enriched_polyhedron<kernel, items> *pMesh)
	{
		m_min[0] = pMesh->xmin(); m_max[0] = pMesh->xmax();
		m_min[1] = pMesh->ymin(); m_max[1] = pMesh->ymax();
		m_min[2] = pMesh->zmin(); m_max[2] = pMesh->zmax();
	}

	// degree	of a face
	static unsigned int degree(Facet_handle pFace)
	{
		return (unsigned)CGAL::circulator_size(pFace->facet_begin());
	}

	// get any border	halfedge attached	to a vertex
	Halfedge_handle	get_border_halfedge(Vertex_handle	v)
	{
		Halfedge_around_vertex_circulator	he = v->vertex_begin();
		Halfedge_around_vertex_circulator	end = he;
		CGAL_For_all(he, end)
			if (he->is_border())
				return he;
		return NULL;
	}


	double **alloc_matrix(const unsigned int nbrows,
		const unsigned int nbcols)
	{
		double **ppMatrix = new double*[nbrows];
		for (unsigned int d = 0; d<nbrows; d++)
			ppMatrix[d] = new double[nbcols];
		return ppMatrix;
	}

	void free_matrix(double **ppMatrix,
		const unsigned int nbrows)
	{
		for (unsigned int d = 0; d<nbrows; d++)
			delete[] ppMatrix[d];
		delete[] ppMatrix;
	}


	void mult_matrix_vector(double **A,
		std::vector<double>& x,
		std::vector<double>& res)
	{
		unsigned int dim = x.size();
		ASSERT(res.size() == dim);
		for (unsigned int r = 0; r<dim; r++)
		{
			res[r] = 0.0;
			for (unsigned int c = 0; c<dim; c++)
				res[r] += A[r][c] * x[c];
		}
	}


	// tag all halfedges
	void tag_halfedges(const int tag)
	{
		for (Halfedge_iterator he = halfedges_begin();
			he != halfedges_end();
			he++)
			he->tag() = tag;
	}

	// tag all facets
	void tag_facets(const	int	tag)
	{
		for (Facet_iterator pFace = facets_begin();
			pFace != facets_end();
			pFace++)
			pFace->tag() = tag;
	}

	// tag all vertices
	void tag_vertices(const	int	tag)
	{
		for (Vertex_iterator	v = vertices_begin();
			v != vertices_end();
			v++)
			v->tag() = tag;
	}

	// set index for all vertices
	void set_index_vertices()
	{
		int	index = 0;
		for (Vertex_iterator	pVertex = vertices_begin();
			pVertex != vertices_end();
			pVertex++)
			pVertex->tag() = index++;
	}

	// is pure degree ?
	bool is_pure_degree(unsigned int d)
	{
		for (Facet_iterator pFace = facets_begin();
			pFace != facets_end();
			pFace++)
			if (degree(pFace) != d)
				return false;
		return true;
	}

	// compute type
	void compute_type()
	{
		m_pure_quad = is_pure_degree(4);
		m_pure_triangle = is_pure_degree(3);
	}

	// compute facet center
	void compute_facet_center(Facet_handle pFace,
		Point& center)
	{
		Halfedge_around_facet_circulator he = pFace->facet_begin();
		Halfedge_around_facet_circulator end = he;
		Vector vec(0.0, 0.0, 0.0);
		int	degree = 0;
		CGAL_For_all(he, end)
		{
			vec = vec + (he->vertex()->point() - CGAL::ORIGIN);
			degree++;
		}
		center = CGAL::ORIGIN + (vec / (kernel::FT)degree);
	}

	// compute average edge length around a vertex
	FT average_edge_length_around(Vertex_handle pVertex)
	{
		FT sum = 0.0;
		Halfedge_around_vertex_circulator he = pVertex->vertex_begin();
		Halfedge_around_vertex_circulator end = he;
		Vector vec(0.0, 0.0, 0.0);
		int	degree = 0;
		CGAL_For_all(he, end)
		{
			Vector vec = he->vertex()->point() -
				he->opposite()->vertex()->point();
			sum += std::sqrt(vec*vec);
			degree++;
		}
		return sum / (FT)degree;
	}




	void gl_draw_arrow(const Point& a,
		const Point& b,
		const FT radius)
	{
		::glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		GLUquadricObj* pQuadric = gluNewQuadric();
		Vector u = b - a;
		FT len = std::sqrt(u*u);
		u = u / len; // normalize
		double angle;
		if (std::sqrt(u.x()*u.x() + u.y()*u.y()) > 1)
			angle = 90.0;
		else
			angle = asin(std::sqrt(u.x()*u.x() + u.y()*u.y())) / 3.14159 * 180.0;
		if (u.z() < 0.0)
			angle = 180.0 - angle;
		Vector rot(u.y(), -u.x(), 0.0);
		Point middle = CGAL::midpoint(a, b);

		// the main tube
		glPushMatrix();
		glTranslated(a.x(), a.y(), a.z());
		glPushMatrix();
		glRotated(-angle, rot.x(), rot.y(), rot.z());
		gluCylinder(pQuadric, radius, radius, len / 2, 10, 1);
		glPopMatrix();
		glPopMatrix();

		// the arrow (cone)
		glPushMatrix();
		glTranslated(middle.x(), middle.y(), middle.z());
		glPushMatrix();
		glRotated(-angle, rot.x(), rot.y(), rot.z());
		gluCylinder(pQuadric, 2 * radius, 0.0, len / 2, 32, 1);
		glPopMatrix();
		glPopMatrix();

		gluDeleteQuadric(pQuadric);

	}

	// draw	using	OpenGL commands	(display lists)
	void gl_draw(bool	smooth_shading,
		bool	use_normals)
	{
		// draw	polygons
		Facet_iterator pFacet = facets_begin();
		for (; pFacet != facets_end(); pFacet++)
		{
			// begin polygon assembly
			::glBegin(GL_POLYGON);
			gl_draw_facet(pFacet, smooth_shading, use_normals);
			::glEnd(); // end polygon assembly
		}
		glFlush();
	}

	void gl_draw_facet(Facet_handle pFacet,
		bool smooth_shading,
		bool use_normals)
	{
		// one normal	per	face
		if (use_normals && !smooth_shading)
		{
			const Facet::Normal_3& normal = pFacet->normal();
			::glNormal3d(normal[0], normal[1], normal[2]);
		}

		// revolve around	current	face to	get	vertices
		Halfedge_around_facet_circulator pHalfedge = pFacet->facet_begin();
		do
		{
			// one normal	per	vertex
			if (use_normals &&	smooth_shading)
			{
				const Facet::Normal_3& normal = pHalfedge->vertex()->normal();
				::glNormal3d(normal[0], normal[1], normal[2]);
			}

			// polygon assembly	is performed per vertex
			const Point& point = pHalfedge->vertex()->point();
			::glVertex3d(point[0], point[1], point[2]);
		} while (++pHalfedge != pFacet->facet_begin());
	}

	// superimpose edges
	void superimpose_edges(bool	skip_ordinary_edges = true,
		bool	skip_control_edges = false,
		bool voronoi_edge = false)
	{
		::glBegin(GL_LINES);
		for (Edge_iterator h = edges_begin();
			h != edges_end();
			h++)
		{
			// ignore	this edges
			if (skip_ordinary_edges && !h->control_edge())
				continue;

			// ignore	control	edges
			if (skip_control_edges	&& h->control_edge())
				continue;

			if (voronoi_edge)
			{
				Facet_handle pFace1 = h->facet();
				Facet_handle pFace2 = h->opposite()->facet();
				if (pFace1 == NULL || pFace2 == NULL)
					continue;

				const Point &p1 = h->vertex()->point();
				const Point &p2 = h->next()->vertex()->point();
				const Point &p3 = h->next()->next()->vertex()->point();

				kernel k;
				Point d1 = k.construct_circumcenter_3_object()(p1, p2, p3);
				::glVertex3d(d1[0], d1[1], d1[2]);

				const Point &pp1 = h->opposite()->vertex()->point();
				const Point &pp2 = h->opposite()->next()->vertex()->point();
				const Point &pp3 = h->opposite()->next()->next()->vertex()->point();
				Point d2 = k.construct_circumcenter_3_object()(pp1, pp2, pp3);
				::glVertex3d(d2[0], d2[1], d2[2]);
			}
			else
			{
				// assembly	and	draw line	segment
				const Point& p1 = h->prev()->vertex()->point();
				const Point& p2 = h->vertex()->point();
				::glVertex3d(p1[0], p1[1], p1[2]);
				::glVertex3d(p2[0], p2[1], p2[2]);
			}
		}
		::glEnd();
	}

	// superimpose vertices
	void superimpose_vertices()
	{
		::glBegin(GL_POINTS);
		for (Point_iterator pPoint = points_begin();
			pPoint != points_end();
			pPoint++)
			::glVertex3d(pPoint->x(), pPoint->y(), pPoint->z());
		::glEnd(); //	end point assembly
	}

	// superimpose vertices
	void superimpose_spheres(double scale)
	{
		GLUquadricObj* pQuadric = gluNewQuadric();
		::glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

		for (Vertex_iterator pVertex = vertices_begin();
			pVertex != vertices_end();
			pVertex++)
		{
			::glPushMatrix();
			double radius = average_edge_length_around(pVertex);
			::glTranslated(pVertex->point().x(),
				pVertex->point().y(),
				pVertex->point().z());
			::gluSphere(pQuadric, scale*radius, 24, 24);
			::glPopMatrix();
		}
		gluDeleteQuadric(pQuadric);
	}



	double duration(const double start)
	{
		return (double)((clock() - start) / CLOCKS_PER_SEC);
	}
	void gl_draw_sphere(Vertex_handle v,
		unsigned char r,
		unsigned char g,
		unsigned char b,
		const double scale)
	{
		GLUquadricObj* pQuadric = gluNewQuadric();
		glColor3ub(r, g, b);
		::glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		double radius = average_edge_length_around(v);
		const Point& p = v->point();
		::glPushMatrix();
		::glTranslated(p.x(), p.y(), p.z());
		::gluSphere(pQuadric, scale*radius, 24, 24);
		::glPopMatrix();
		::gluDeleteQuadric(pQuadric);
	}

	// write in	obj	file format	(OBJ).
	void write_obj(char	*pFilename,
		int incr = 1) //	1-based	by default
	{
		std::ofstream	stream(pFilename);

		// output	vertices
		for (Point_iterator pPoint = points_begin();
			pPoint != points_end();
			pPoint++)
			stream << 'v' << ' ' << pPoint->x() << ' ' <<
			pPoint->y() << ' ' <<
			pPoint->z() << std::endl;

		// precompute	vertex indices
		this->set_index_vertices();

		// output	facets
		for (Facet_iterator pFacet = facets_begin();
			pFacet != facets_end();
			pFacet++)
		{
			stream << 'f';
			Halfedge_around_facet_circulator pHalfedge = pFacet->facet_begin();
			do
				stream << '	' << pHalfedge->vertex()->tag() + incr;
			while (++pHalfedge != pFacet->facet_begin());
			stream << std::endl;
		}
	}

	// draw bounding box
	void gl_draw_bounding_box()
	{
		::glBegin(GL_LINES);
		// along x axis
		::glVertex3d(m_min[0], m_min[1], m_min[2]);
		::glVertex3d(m_max[0], m_min[1], m_min[2]);
		::glVertex3d(m_min[0], m_min[1], m_max[2]);
		::glVertex3d(m_max[0], m_min[1], m_max[2]);
		::glVertex3d(m_min[0], m_max[1], m_min[2]);
		::glVertex3d(m_max[0], m_max[1], m_min[2]);
		::glVertex3d(m_min[0], m_max[1], m_max[2]);
		::glVertex3d(m_max[0], m_max[1], m_max[2]);
		// along y axis
		::glVertex3d(m_min[0], m_min[1], m_min[2]);
		::glVertex3d(m_min[0], m_max[1], m_min[2]);
		::glVertex3d(m_min[0], m_min[1], m_max[2]);
		::glVertex3d(m_min[0], m_max[1], m_max[2]);
		::glVertex3d(m_max[0], m_min[1], m_min[2]);
		::glVertex3d(m_max[0], m_max[1], m_min[2]);
		::glVertex3d(m_max[0], m_min[1], m_max[2]);
		::glVertex3d(m_max[0], m_max[1], m_max[2]);
		// along z axis
		::glVertex3d(m_min[0], m_min[1], m_min[2]);
		::glVertex3d(m_min[0], m_min[1], m_max[2]);
		::glVertex3d(m_min[0], m_max[1], m_min[2]);
		::glVertex3d(m_min[0], m_max[1], m_max[2]);
		::glVertex3d(m_max[0], m_min[1], m_min[2]);
		::glVertex3d(m_max[0], m_min[1], m_max[2]);
		::glVertex3d(m_max[0], m_max[1], m_min[2]);
		::glVertex3d(m_max[0], m_max[1], m_max[2]);
		::glEnd();
	}

	// count #boundaries
	unsigned int nb_boundaries()
	{
		unsigned int nb = 0;
		tag_halfedges(0);
		m_seed_boundaries.clear();
		Halfedge_handle	seed_halfedge = NULL;
		while ((seed_halfedge = get_border_halfedge_tag(0)) != NULL)
		{
			m_seed_boundaries.push_back(seed_halfedge);
			nb++;
			seed_halfedge->tag() = 1;
			Vertex_handle	seed_vertex = seed_halfedge->prev()->vertex();
			Halfedge_handle	current_halfedge = seed_halfedge;
			Halfedge_handle	next_halfedge;
			do
			{
				next_halfedge = current_halfedge->next();
				next_halfedge->tag() = 1;
				current_halfedge = next_halfedge;
			} while (next_halfedge->prev()->vertex() != seed_vertex);
		}
		m_b = nb;
		return nb;
	}

	// get any border	halfedge with	tag
	Halfedge_handle	get_border_halfedge_tag(int	tag)
	{
		for (Halfedge_iterator pHalfedge = halfedges_begin();
			pHalfedge != halfedges_end();
			pHalfedge++)
			if (pHalfedge->is_border() &&
				pHalfedge->tag() == tag)
				return pHalfedge;
		return NULL;
	}

	// get any facet with	tag
	Facet_handle get_facet_tag(const int tag)
	{
		for (Facet_iterator pFace = facets_begin();
			pFace != facets_end();
			pFace++)
			if (pFace->tag() == tag)
				return pFace;
		return NULL;
	}

	// tag component 
	void tag_component(Facet_handle	pSeedFacet,
		const int tag_free,
		const int tag_done)
	{
		pSeedFacet->tag() = tag_done;
		std::list<Facet_handle> facets;
		facets.push_front(pSeedFacet);
		while (!facets.empty())
		{
			Facet_handle pFacet = facets.front();
			facets.pop_front();
			pFacet->tag() = tag_done;
			Halfedge_around_facet_circulator pHalfedge = pFacet->facet_begin();
			Halfedge_around_facet_circulator end = pHalfedge;
			CGAL_For_all(pHalfedge, end)
			{
				Facet_handle pNFacet = pHalfedge->opposite()->facet();
				if (pNFacet != NULL && pNFacet->tag() == tag_free)
				{
					facets.push_front(pNFacet);
					pNFacet->tag() = tag_done;
				}
			}
		}
	}

	// count #components
	unsigned int nb_components()
	{
		unsigned int nb = 0;
		tag_facets(0);
		Facet_handle seed_facet = NULL;
		while ((seed_facet = get_facet_tag(0)) != NULL)
		{
			nb++;
			tag_component(seed_facet, 0, 1);
		}
		m_c = nb;
		return nb;
	}


	// compute the genus
	// V - E + F + B = 2 (C	-	G)
	// C ->	#connected components
	// G : genus
	// B : #boundaries
	int	genus()
	{
		int	c = nb_components();
		int	b = nb_boundaries();
		int	v = size_of_vertices();
		int	e = size_of_halfedges() / 2;
		int	f = size_of_facets();
		m_g = genus(c, v, f, e, b);
		return m_g;
	}
	int	genus(int	c,
		int	v,
		int	f,
		int	e,
		int	b)
	{
		m_g = (2 * c + e - b - f - v) / 2;
		return m_g;
	}

	// return cotan at corner specified by he->vertex()
	FT cotangent(Halfedge_handle he)
	{
		Vector pa = he->prev()->vertex()->point() - he->vertex()->point();
		Vector pb = he->next()->vertex()->point() - he->vertex()->point();
		// (pa . pb)/((pa x pb).len)
		FT dot = pa * pb;
		Vector cross = CGAL::cross_product(pa, pb);
		FT norm = (FT)std::sqrt(cross*cross);
		if (norm != 0.0)
			return dot / norm;
		else
			return (FT)0.0; // undefined
	}

	// compute cotan weights on each edge
	void compute_cotan_weights()
	{
		for (Edge_iterator he = edges_begin();
			he != edges_end();
			he++)
		{
			FT w = cotangent(he->next()) +
				cotangent(he->opposite()->next());
			w = (w < 0) ? 0 : w; // TODO
			he->cotan() = w;
			he->opposite()->cotan() = w;
		}
	}

};

// compute facet normal	
struct Facet_normal	// (functor)
{
	template <class	Facet>
	void operator()(Facet& f)
	{
		typename Facet::Normal_3 sum = CGAL::NULL_VECTOR;
		typename Facet::Halfedge_around_facet_circulator h = f.facet_begin();
		do
		{
			typename Facet::Normal_3 normal = CGAL::cross_product(
				h->next()->vertex()->point() - h->vertex()->point(),
				h->next()->next()->vertex()->point() - h->next()->vertex()->point());
			double sqnorm = normal * normal;
			if (sqnorm != 0)
				normal = normal / (float)std::sqrt(sqnorm);
			sum = sum + normal;
		} while (++h != f.facet_begin());
		double sqnorm = sum * sum;
		if (sqnorm != 0.0)
			f.normal() = sum / std::sqrt(sqnorm);
		else
		{
			f.normal() = CGAL::NULL_VECTOR;
		}
	}
};


// compute vertex	normal 
struct Vertex_normal //	(functor)
{
	template <class	Vertex>
	void operator()(Vertex&	v)
	{
		typename Vertex::Normal	normal = CGAL::NULL_VECTOR;
		Vertex::Halfedge_around_vertex_const_circulator	pHalfedge = v.vertex_begin();
		Vertex::Halfedge_around_vertex_const_circulator	begin = pHalfedge;
		CGAL_For_all(pHalfedge, begin)
			if (!pHalfedge->is_border())
				normal = normal + pHalfedge->facet()->normal();
		double	sqnorm = normal * normal;
		if (sqnorm != 0.0f)
			v.normal() = normal / (float)std::sqrt(sqnorm);
		else
			v.normal() = CGAL::NULL_VECTOR;
	}
};

#endif //	_POLYGON_MESH_