// Supported with union (c) 2020 Union team

// User API for zCModel
// Add your methods here

inline int zCModel::IsAniActive( int aniID ) {
	return IsAniActive( GetAniFromAniID( aniID ) );
};

inline zBOOL zCModel::IsAniActive( const zSTRING& aniName ) {
  return IsAniActive( GetAniIDFromAniName( aniName ) );
};