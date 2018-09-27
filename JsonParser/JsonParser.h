#pragma once
#include "cSingletone.h"

class JsonParser : public cSingletone<JsonParser>
{
public:
	void ParseValue(const string fileName, Json::Value* root) const;
	void WriteOnFile(string fileName, Json::Value* root) const;

private:
	JsonParser();
	string ReadFromFile(const string fileName) const;
private:
	friend class cSingletone<JsonParser>;
};

