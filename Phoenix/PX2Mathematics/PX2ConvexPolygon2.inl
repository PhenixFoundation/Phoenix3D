/*
*
* 文件名称	：	PX2ConvexPolygon2.inl
*
*/

//----------------------------------------------------------------------------
template <typename Real>
ConvexPolygon2<Real>::ConvexPolygon2 (int numVertices,
									  Vector2<Real>* vertices, NCLine* lines)
									  :
Polygon2<Real>(numVertices, vertices)
{
	if (lines)
	{
		mLines = lines;
	}
	else
	{
		mLines = new1<NCLine>(mNumVertices);
		UpdateLines();
	}
}
//----------------------------------------------------------------------------
template <typename Real>
ConvexPolygon2<Real>::ConvexPolygon2 (const ConvexPolygon2& polygon)
:
Polygon2<Real>(polygon)
{
	mLines = 0;
	*this = polygon;
}
//----------------------------------------------------------------------------
template <typename Real>
ConvexPolygon2<Real>::~ConvexPolygon2 ()
{
}
//----------------------------------------------------------------------------
template <typename Real>
ConvexPolygon2<Real>& ConvexPolygon2<Real>::operator= (
	const ConvexPolygon2& polygon)
{
	Polygon2<Real>::operator=(polygon);
	mLines = polygon.mLines;
	mSharingEdges = polygon.mSharingEdges;
	return *this;
}
//----------------------------------------------------------------------------
template <typename Real>
const typename ConvexPolygon2<Real>::NCLine*
ConvexPolygon2<Real>::GetLines () const
{
	return mLines;
}
//----------------------------------------------------------------------------
template <typename Real>
const typename ConvexPolygon2<Real>::NCLine&
ConvexPolygon2<Real>::GetLine (int i) const
{
	assertion(0 <= i && i < mNumVertices, "Invalid index in GetLine\n");
	return mLines[i];
}
//----------------------------------------------------------------------------
template <typename Real>
void ConvexPolygon2<Real>::SetVertex (int i, const Vector2<Real>& vertex)
{
	Polygon2<Real>::SetVertex(i, vertex);

	mSharingEdges.insert((i - 1) % mNumVertices);
	mSharingEdges.insert(i);
}
//----------------------------------------------------------------------------
template <typename Real>
void ConvexPolygon2<Real>::UpdateLines ()
{
	if (mSharingEdges.size() > 0)
	{
		Vector2<Real> average = ComputeVertexAverage();
		for (int i = 0; i < mNumVertices; ++i)
		{
			UpdateLine(i, average);
		}
	}
}
//----------------------------------------------------------------------------
template <typename Real>
bool ConvexPolygon2<Real>::IsConvex (Real threshold) const
{
	Real maxDistance = -Math<Real>::MAX_REAL;
	Real minDistance = Math<Real>::MAX_REAL;

	for (int j = 0; j < mNumVertices; ++j)
	{
		const NCLine& line = mLines[j];
		for (int i = 0; i < mNumVertices; ++i)
		{
			Real distance = line.first.Dot(mVertices[i]) - line.second;
			if (distance < minDistance)
			{
				minDistance = distance;
			}
			if (distance > maxDistance)
			{
				maxDistance = distance;
			}
			if (distance < threshold)
			{
				return false;
			}
		}
	}

	return true;
}
//----------------------------------------------------------------------------
template <typename Real>
bool ConvexPolygon2<Real>::Contains (const Vector2<Real>& p,
									 Real threshold) const
{
	for (int i = 0; i < mNumVertices; ++i)
	{
		const NCLine& line = mLines[i];
		Real distance = line.first.Dot(p) - line.second;
		if (distance < threshold)
		{
			return false;
		}
	}
	return true;
}
//----------------------------------------------------------------------------
template <typename Real>
void ConvexPolygon2<Real>::UpdateLine (int i, const Vector2<Real>& average)
{
	Vector2<Real>& vertex0 = mVertices[i];
	Vector2<Real>& vertex1 = mVertices[(i + 1) % mNumVertices];

	Vector2<Real> diff = average - vertex0;
	Vector2<Real> edge = vertex1 - vertex0;
	Vector2<Real> normal = -edge.Perp();
	Real length = normal.Length();
	if (length > Math<Real>::ZERO_TOLERANCE)
	{
		normal /= length;
		Real dot = normal.Dot(diff);
		assertion(dot >= (Real)0, "Dot product must be nonnegative\n");
		if (dot < (Real)0)
		{
			normal = -normal;
		}
	}
	else
	{
		// 边是退化的，法线指向中点
		normal = diff;
		normal.Normalize();
	}

	// This line has an inner-pointing normal.
	mLines[i].first = normal;
	mLines[i].second = normal.Dot(vertex0);
}
//----------------------------------------------------------------------------
