#pragma once

typedef float vec_t;

class vector2 {
public:
	vec_t x, y;

#pragma region constructors

	vector2( ) {
		x = y = 0.f;
	}

	vector2( vec_t a, vec_t b ) {
		x = a;
		y = b;
	}

	vector2( vec_t a ) {
		x = a;
		y = 0.f;
	}

	vector2( vec_t* f ) {
		x = f[ 0 ];
		y = f[ 1 ];
	}
#pragma endregion

#pragma region operators

	bool operator==( const vector2& v ) {
		return x == v.x && y == v.y;
	}
	bool operator!=( const vector2& v ) {
		return x != v.x || y != v.y;
	}

	vector2 operator+( const vector2& v ) {
		vector2 other = *this;
		other.x += v.x; other.y += v.y;

		return other;
	}

	vector2 operator-( const vector2& v ) {
		vector2 other = *this;
		other.x -= v.x; other.y -= v.y;

		return other;
	}

	vector2 operator*( const float& v ) {
		vector2 other = *this;
		other.x *= v; other.y *= v;

		return other;
	}

	vector2& operator+=( const vector2& v ) {
		x += v.x; y += v.y;
		return *this;
	}

	vector2& operator-=( const vector2& v ) {
		x -= v.x; y -= v.y;
		return *this;
	}

	vector2& operator*=( float fl ) {
		x *= fl;
		y *= fl;
		return *this;
	}

	vector2& operator*=( const vector2& v ) {
		x *= v.x;
		y *= v.y;
		return *this;
	}

	vector2& operator/=( const vector2& v ) {
		x /= v.x;
		y /= v.y;
		return *this;
	}

	vector2& operator+=( float fl ) {
		x += fl;
		y += fl;
		return *this;
	}

	vector2& operator+( float fl ) {
		x += fl;
		y += fl;
		return *this;
	}

	vector2& operator-( float fl ) {
		x -= fl;
		y -= fl;
		return *this;
	}

	vector2& operator/=( float fl ) {
		x /= fl;
		y /= fl;
		return *this;
	}
	vector2& operator-=( float fl ) {
		x -= fl;
		y -= fl;

		return *this;
	}

#pragma endregion

	vec_t length( ) {
		return sqrtf( x * x + y * y );
	}

	vec_t distance( vector2& v ) {
		return ( *this - v ).length( );
	}

	void normalize( ) {
		vec_t l = length( );
		if ( l <= 0 ) return;

		x /= l;
		y /= l;
	}

	void invalidate( ) {
		x = y = std::numeric_limits<float>::infinity( );
	}

	bool is_valid( ) {
		return std::isfinite( x ) && std::isfinite( y );
	}
};

class vector3 {
public:
	vec_t x, y, z;

#pragma region constructors

	vector3( ) {
		x = y = z = 0.f;
	}

	vector3( vec_t a, vec_t b, vec_t c ) {
		x = a;
		y = b;
		z = c;
	}

	vector3( vec_t a, vec_t b ) {
		x = a;
		y = b;
		z = 0.f;
	}

	vector3( vec_t a ) {
		x = a;
		y = z = 0.f;
	}

	vector3( vec_t* f ) {
		x = f[ 0 ];
		y = f[ 1 ];
		z = f[ 2 ];
	}
#pragma endregion

#pragma region operators

	bool operator==( const vector3& v ) {
		return x == v.x && y == v.y && z == v.z;
	}
	bool operator!=( const vector3& v ) {
		return x != v.x || y != v.y || z != v.z;
	}

	bool operator==( const vector2& v ) {
		return x == v.x && y == v.y;
	}
	bool operator!=( const vector2& v ) {
		return x != v.x || y != v.y;
	}

	vector3 operator+( const vector3& v ) {
		vector3 other = *this;
		other.x += v.x; other.y += v.y; other.z += v.z;

		return other;
	}

	vector3 operator+( const vector2& v ) {
		vector3 other = *this;
		other.x += v.x; other.y += v.y;

		return other;
	}

	vector3 operator-( const vector3& v ) {
		vector3 other = *this;
		other.x -= v.x; other.y -= v.y; other.z -= v.z;

		return other;
	}

	vector3 operator-( const vector2& v ) {
		vector3 other = *this;
		other.x -= v.x; other.y -= v.y;

		return other;
	}

	vector3 operator*( const float& v ) {
		vector3 other = *this;
		other.x *= v; other.y *= v; other.z *= v;

		return other;
	}

	vector3& operator+=( const vector3& v ) {
		x += v.x; y += v.y; z += v.z;
		return *this;
	}

	vector3& operator+=( const vector2& v ) {
		x += v.x; y += v.y;
		return *this;
	}

	vector3& operator-=( const vector3& v ) {
		x -= v.x; y -= v.y; z -= v.z;
		return *this;
	}

	vector3& operator-=( const vector2& v ) {
		x -= v.x; y -= v.y;
		return *this;
	}

	vector3& operator*=( float fl ) {
		x *= fl;
		y *= fl;
		z *= fl;
		return *this;
	}

	vector3& operator*=( const vector3& v ) {
		x *= v.x;
		y *= v.y;
		z *= v.z;
		return *this;
	}

	vector3& operator*=( const vector2& v ) {
		x *= v.x;
		y *= v.y;
		return *this;
	}

	vector3& operator/=( const vector3& v ) {
		x /= v.x;
		y /= v.y;
		z /= v.z;
		return *this;
	}

	vector3& operator/=( const vector2& v ) {
		x /= v.x;
		y /= v.y;
		return *this;
	}

	vector3& operator+=( float fl ) {
		x += fl;
		y += fl;
		z += fl;
		return *this;
	}

	vector3& operator+( float fl ) {
		x += fl;
		y += fl;
		z += fl;
		return *this;
	}

	vector3& operator-( float fl ) {
		x -= fl;
		y -= fl;
		z -= fl;
		return *this;
	}

	vector3& operator/=( float fl ) {
		x /= fl;
		y /= fl;
		z /= fl;
		return *this;
	}
	vector3& operator-=( float fl ) {
		x -= fl;
		y -= fl;
		z -= fl;

		return *this;
	}

#pragma endregion

	vec_t length( ) {
		return sqrtf( x * x + y * y + z * z );
	}

	vec_t distance( vector3& v ) {
		return ( *this - v ).length( );
	}

	void normalize( ) {
		vec_t l = length( );
		if ( l <= 0 ) return;

		x /= l;
		y /= l;
		z /= l;
	}

	void invalidate( ) {
		x = y = z = std::numeric_limits<float>::infinity( );
	}

	bool is_valid( ) {
		return std::isfinite( x ) && std::isfinite( y ) && std::isfinite( z );
	}
};