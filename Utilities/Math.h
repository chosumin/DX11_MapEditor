#pragma once
class Math
{
public:
	static const float PI;

	static float ToRadian(float degree);
	static float ToDegree(float radian);

	static int Random(int r1, int r2);
	static float Random(float r1, float r2);
};