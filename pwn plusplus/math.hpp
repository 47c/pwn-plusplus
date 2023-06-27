#pragma once

#define M_PI			3.14159265358979323846
#define M_PI_F		((float)(M_PI))
#define RAD2DEG( x )  ( (float)(x) * (float)(180.f / M_PI_F) )
#define DEG2RAD( x )  ( (float)(x) * (float)(M_PI_F / 180.f) )

namespace math {
	inline void sin_cos( float radians, float* sine, float* cosine ) {
		*sine = sin( radians );
		*cosine = cos( radians );
	}

	inline vector3 angle_vector( vector3& angles ) {
		float sp, sy, cp, cy;

		sin_cos( DEG2RAD( angles.y ), &sy, &cy );
		sin_cos( DEG2RAD( angles.x ), &sp, &cp );

		return vector3(cp * cy, cp * sy, -sp);
	}

	inline vector3 vector_angle( const vector3& forward ) {
		float tmp, yaw, pitch;

		if ( forward.y == 0 && forward.x == 0 ) {
			yaw = 0;
			if ( forward.z > 0 )
				pitch = 270;
			else
				pitch = 90;
		} else {
			yaw = float( atan2( forward.y, forward.x ) * 180 / M_PI );
			if ( yaw < 0 )
				yaw += 360;

			tmp = sqrt( forward.x * forward.x + forward.y * forward.y );
			pitch = float( atan2( -forward.z, tmp ) * 180 / M_PI );
			if ( pitch < 0 )
				pitch += 360;
		}

		return vector3( pitch, yaw );
	}

	inline float interpolate( float a, float b, float c ) {
		if ( fabsf( b - a ) <= 2 * c )
			return b;

		return  a + ( b - a ) * c ;
	}
}