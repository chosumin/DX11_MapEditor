#include "stdafx.h"
#include "JsonParser.h"

JsonParser::JsonParser()
{
}

void JsonParser::ParseValue(const string fileName, Json::Value * root) const
{
	string buffer = ReadFromFile(fileName);
	Json::Reader reader;

	bool readerSuccess = reader.parse(buffer, *root);
	assert(readerSuccess && "Json ÆÄ½Ì ¿À·ù!");
}

void JsonParser::WriteOnFile(string fileName, Json::Value * root) const
{
	Json::StyledWriter writer;
	string output = writer.write(*root);

	ofstream out(fileName);

	out << output << endl;
	/*FILE* fp = OpenFile(fileName, "wb");

	fwrite(buffer.c_str(), 1, length, fp);
	fclose(fp);*/

	out.close();
}

string JsonParser::ReadFromFile(const string fileName) const
{
	string buffer = "";
	ifstream in(fileName);
	if (!in.is_open()) assert(false);

	in.seekg(0, ios::end);
	int size = in.tellg();
	buffer.resize(size);
	in.seekg(0, ios::beg);
	in.read(&buffer[0], size);

	in.close();

	return buffer;
}
