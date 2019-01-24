#pragma once
#include <xnamath.h>
struct SimpleVertex
{
	XMFLOAT3 Pos;
	XMFLOAT2 Tex;
};

struct CBNeverChanges
{
	XMMATRIX mView;
};

struct CBChangeOnResize
{
	XMMATRIX mProjection;
};

struct CBChangesEveryFrame
{
	XMMATRIX mWorld;
	XMFLOAT4 vMeshColor;
};

enum KeyType
{
	LEFT_PRESSED = 0,
	RIGHT_PRESSED = 1,
	UP_PRESSED = 2,
	DOWN_PRESSED = 3,
	MOUSE_LEFT_PRESSED = 4,
	MOUSE_RIGHT_PRESSED = 5
};