#pragma once

class IClonable
{
public:
	virtual void Clone(void** clone) = 0;
};