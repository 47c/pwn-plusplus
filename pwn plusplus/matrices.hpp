#pragma once

class matrix3x4_t {


public:
	matrix3x4_t( ) { }
	matrix3x4_t(
		float m00, float m01, float m02, float m03,
		float m10, float m11, float m12, float m13,
		float m20, float m21, float m22, float m23 ) {
		m_flMatVal[ 0 ][ 0 ] = m00;	m_flMatVal[ 0 ][ 1 ] = m01; m_flMatVal[ 0 ][ 2 ] = m02; m_flMatVal[ 0 ][ 3 ] = m03;
		m_flMatVal[ 1 ][ 0 ] = m10;	m_flMatVal[ 1 ][ 1 ] = m11; m_flMatVal[ 1 ][ 2 ] = m12; m_flMatVal[ 1 ][ 3 ] = m13;
		m_flMatVal[ 2 ][ 0 ] = m20;	m_flMatVal[ 2 ][ 1 ] = m21; m_flMatVal[ 2 ][ 2 ] = m22; m_flMatVal[ 2 ][ 3 ] = m23;
	}
	//-----------------------------------------------------------------------------
	// Creates a matrix where the X axis = forward
	// the Y axis = left, and the Z axis = up
	//-----------------------------------------------------------------------------
	void Init( const vector3& xAxis, const vector3& yAxis, const vector3& zAxis, const vector3& vecOrigin ) {
		m_flMatVal[ 0 ][ 0 ] = xAxis.x; m_flMatVal[ 0 ][ 1 ] = yAxis.x; m_flMatVal[ 0 ][ 2 ] = zAxis.x; m_flMatVal[ 0 ][ 3 ] = vecOrigin.x;
		m_flMatVal[ 1 ][ 0 ] = xAxis.y; m_flMatVal[ 1 ][ 1 ] = yAxis.y; m_flMatVal[ 1 ][ 2 ] = zAxis.y; m_flMatVal[ 1 ][ 3 ] = vecOrigin.y;
		m_flMatVal[ 2 ][ 0 ] = xAxis.z; m_flMatVal[ 2 ][ 1 ] = yAxis.z; m_flMatVal[ 2 ][ 2 ] = zAxis.z; m_flMatVal[ 2 ][ 3 ] = vecOrigin.z;
	}

	//-----------------------------------------------------------------------------
	// Creates a matrix where the X axis = forward
	// the Y axis = left, and the Z axis = up
	//-----------------------------------------------------------------------------
	matrix3x4_t( const vector3& xAxis, const vector3& yAxis, const vector3& zAxis, const vector3& vecOrigin ) {
		Init( xAxis, yAxis, zAxis, vecOrigin );
	}

	inline void SetOrigin( vector3 const& p ) {
		m_flMatVal[ 0 ][ 3 ] = p.x;
		m_flMatVal[ 1 ][ 3 ] = p.y;
		m_flMatVal[ 2 ][ 3 ] = p.z;
	}

	inline bool IsEmpty( ) {
		if ( m_flMatVal[ 0 ][ 0 ] == 0.f && m_flMatVal[ 0 ][ 1 ] == 0.f && m_flMatVal[ 0 ][ 2 ] == 0.f && m_flMatVal[ 0 ][ 3 ] == 0.f &&
			 m_flMatVal[ 1 ][ 0 ] == 0.f && m_flMatVal[ 1 ][ 1 ] == 0.f && m_flMatVal[ 1 ][ 2 ] == 0.f && m_flMatVal[ 1 ][ 3 ] == 0.f &&
			 m_flMatVal[ 2 ][ 0 ] == 0.f && m_flMatVal[ 2 ][ 1 ] == 0.f && m_flMatVal[ 2 ][ 2 ] == 0.f && m_flMatVal[ 2 ][ 3 ] == 0.f )
			return true;
		return false;
	}

	inline void Invalidate( void ) {
		for ( int i = 0; i < 3; i++ ) {
			for ( int j = 0; j < 4; j++ ) {
				m_flMatVal[ i ][ j ] = std::numeric_limits<float>::infinity( );;
			}
		}
	}

	float* operator[]( int i ) { return m_flMatVal[ i ]; }
	const float* operator[]( int i ) const { return m_flMatVal[ i ]; }
	float* Base( ) { return &m_flMatVal[ 0 ][ 0 ]; }
	const float* Base( ) const { return &m_flMatVal[ 0 ][ 0 ]; }

	float m_flMatVal[ 3 ][ 4 ];
};
class matrix3x4a_t :public matrix3x4_t {
public:
	/*
	matrix3x4a_t() { if (((size_t)Base()) % 16 != 0) { Error( "matrix3x4a_t missaligned" ); } }
	*/
	matrix3x4a_t& operator=( const matrix3x4_t& src ) { memcpy( Base( ), src.Base( ), sizeof( float ) * 3 * 4 ); return *this; };
};
class VMatrix {
public:

	VMatrix( );
	VMatrix(
		vec_t m00, vec_t m01, vec_t m02, vec_t m03,
		vec_t m10, vec_t m11, vec_t m12, vec_t m13,
		vec_t m20, vec_t m21, vec_t m22, vec_t m23,
		vec_t m30, vec_t m31, vec_t m32, vec_t m33
	);

	// Creates a matrix where the X axis = forward
	// the Y axis = left, and the Z axis = up
	VMatrix( const vector3& forward, const vector3& left, const vector3& up );

	// Construct from a 3x4 matrix
	VMatrix( const matrix3x4_t& matrix3x4 );

	// Set the values in the matrix.
	void		Init(
		vec_t m00, vec_t m01, vec_t m02, vec_t m03,
		vec_t m10, vec_t m11, vec_t m12, vec_t m13,
		vec_t m20, vec_t m21, vec_t m22, vec_t m23,
		vec_t m30, vec_t m31, vec_t m32, vec_t m33
	);


	// Initialize from a 3x4
	void		Init( const matrix3x4_t& matrix3x4 );

	// array access
	inline float* operator[]( int i ) {
		return m[ i ];
	}

	inline const float* operator[]( int i ) const {
		return m[ i ];
	}

	// Get a pointer to m[0][0]
	inline float* Base( ) {
		return &m[ 0 ][ 0 ];
	}

	inline const float* Base( ) const {
		return &m[ 0 ][ 0 ];
	}

	void		SetLeft( const vector3& vLeft );
	void		SetUp( const vector3& vUp );
	void		SetForward( const vector3& vForward );

	void		GetBasisVectors( vector3& vForward, vector3& vLeft, vector3& vUp ) const;
	void		SetBasisVectors( const vector3& vForward, const vector3& vLeft, const vector3& vUp );

	// Get/set the translation.
	vector3& GetTranslation( vector3& vTrans ) const;
	void		SetTranslation( const vector3& vTrans );

	void		PreTranslate( const vector3& vTrans );
	void		PostTranslate( const vector3& vTrans );

	matrix3x4_t& As3x4( );
	const matrix3x4_t& As3x4( ) const;
	void		CopyFrom3x4( const matrix3x4_t& m3x4 );
	void		Set3x4( matrix3x4_t& matrix3x4 ) const;

	bool		operator==( const VMatrix& src ) const;
	bool		operator!=( const VMatrix& src ) const { return !( *this == src ); }

	// Access the basis vectors.
	vector3		GetLeft( ) const;
	vector3		GetUp( ) const;
	vector3		GetForward( ) const;
	vector3		GetTranslation( ) const;


	// Matrix->vector3 operations.
public:
	// Multiply by a 3D vector3 (same as operator*).
	void		V3Mul( const vector3& vIn, vector3& vOut ) const;

	// Multiply by a 4D vector3.
	//void		V4Mul( const Vector4D &vIn, Vector4D &vOut ) const;

	// Applies the rotation (ignores translation in the matrix). (This just calls VMul3x3).
	vector3		ApplyRotation( const vector3& vVec ) const;

	// Multiply by a vector3 (divides by w, assumes input w is 1).
	vector3		operator*( const vector3& vVec ) const;

	// Multiply by the upper 3x3 part of the matrix (ie: only apply rotation).
	vector3		VMul3x3( const vector3& vVec ) const;

	// Apply the inverse (transposed) rotation (only works on pure rotation matrix)
	vector3		VMul3x3Transpose( const vector3& vVec ) const;

	// Multiply by the upper 3 rows.
	vector3		VMul4x3( const vector3& vVec ) const;

	// Apply the inverse (transposed) transformation (only works on pure rotation/translation)
	vector3		VMul4x3Transpose( const vector3& vVec ) const;


	// Matrix->plane operations.
	//public:
	// Transform the plane. The matrix can only contain translation and rotation.
	//void		TransformPlane( const VPlane &inPlane, VPlane &outPlane ) const;

	// Just calls TransformPlane and returns the result.
	//VPlane		operator*(const VPlane &thePlane) const;

	// Matrix->matrix operations.
public:

	VMatrix& operator=( const VMatrix& mOther );

	// Multiply two matrices (out = this * vm).
	void		MatrixMul( const VMatrix& vm, VMatrix& out ) const;

	// Add two matrices.
	const VMatrix& operator+=( const VMatrix& other );

	// Just calls MatrixMul and returns the result.	
	VMatrix		operator*( const VMatrix& mOther ) const;

	// Add/Subtract two matrices.
	VMatrix		operator+( const VMatrix& other ) const;
	VMatrix		operator-( const VMatrix& other ) const;

	// Negation.
	VMatrix		operator-( ) const;

	// Return inverse matrix. Be careful because the results are undefined 
	// if the matrix doesn't have an inverse (ie: InverseGeneral returns false).
	VMatrix		operator~( ) const;

	// Matrix operations.
public:
	// Set to identity.
	void		Identity( );

	bool		IsIdentity( ) const;

	// Setup a matrix for origin and angles.

	// General inverse. This may fail so check the return!
	bool		InverseGeneral( VMatrix& vInverse ) const;

	// Does a fast inverse, assuming the matrix only contains translation and rotation.
	void		InverseTR( VMatrix& mRet ) const;

	// Usually used for debug checks. Returns true if the upper 3x3 contains
	// unit vectors and they are all orthogonal.
	bool		IsRotationMatrix( ) const;

	// This calls the other InverseTR and returns the result.
	VMatrix		InverseTR( ) const;

	// Get the scale of the matrix's basis vectors.
	vector3		GetScale( ) const;

	// (Fast) multiply by a scaling matrix setup from vScale.
	VMatrix		Scale( const vector3& vScale );

	// Normalize the basis vectors.
	VMatrix		NormalizeBasisVectors( ) const;

	// Transpose.
	VMatrix		Transpose( ) const;

	// Transpose upper-left 3x3.
	VMatrix		Transpose3x3( ) const;

public:
	// The matrix.
	vec_t		m[ 4 ][ 4 ];
};
