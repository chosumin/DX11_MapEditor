#pragma once
#include "stdafx.h"

template<typename T>
class cSingletone
{
public:
	static T* Get()
	{
		if (m_pInstance == nullptr)
		{
			m_pInstance = new T();
		}
		return m_pInstance;
	}

	static void Delete()
	{
		SAFE_DELETE(m_pInstance);
	}
protected:
	virtual ~cSingletone() {}
private:
	static T*	m_pInstance;
};

template<typename T>
T* cSingletone<T>::m_pInstance = nullptr;