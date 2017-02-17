#pragma once
#include <string>
#include <vector>
#include <map>
#include <sstream>
#include <list>
#include <memory>
#include <sstream>

namespace JSON {
	typedef enum {
		jsonStringType = 0, jsonNumberType, jsonObjectType, jsonArrayType, jsonTrueType, jsonFalseType, jsonNullType, jsonWrongName
	}jsonType;
	
	typedef enum {
		PARSE_OK = 0, MISSING_COLON, MISSING_FRONT_BRACKET, MISSING_BACK_BRACKET, MISSING_QUOTATION, MISSING_COMMA, WRONG_VALUE, WRONG_NUMBER,\
		MISSING_NAME, LEFT_CONTENT, INVALID_UNICODE,INVALID_UNICODE_HEX,INVALID_ESC,INVALID_STRING_CHAR
	}parseStat; 

	class jsonStruct
	{
		friend class Json;
	public:
		bool isTrue() { return type == jsonTrueType ? true : false; }
		bool isFalse() { return type == jsonFalseType ? true : false; }
		bool isNull() { return type == jsonNullType ? true : false; }
		bool isNumber() { return type == jsonNumberType ? true : false; }
		bool isString() { return type == jsonStringType ? true : false; }
		bool isObject() { return type == jsonObjectType ? true : false; }
		bool isArray() { return type == jsonArrayType ? true : false; }
		double getNumber() { return number; }
		std::string getString() { return std::string(*string); }
		std::shared_ptr<std::vector<jsonStruct>> getArray() { return array; }  //是否隐藏内部指针？
		std::shared_ptr<std::map<std::string, jsonStruct>> getObject() { return obejct; }  ////是否隐藏内部指针？
		int getType() { return type; }
		jsonStruct operator[](int pos) { 
			jsonStruct res;
			return res = array->at(pos); 
		}
		jsonStruct operator[](std::string name) {
			jsonStruct res;
			auto it = obejct->find(name);
			if (it == obejct->end()) {
				res.type = jsonWrongName;
				return res;
			}
			res = it->second;
			return res;
		}
		jsonStruct operator=(double num) {
			this->type = jsonNumberType;
			this->number = num;
			return *this;
		}

	private:
		std::shared_ptr<std::string> string;
		double number = 0;
		std::shared_ptr<std::vector<jsonStruct>> array;
		std::shared_ptr<std::map<std::string, jsonStruct>> obejct;
		jsonType type=jsonWrongName;
	};

	typedef std::map<std::string, jsonStruct> jsonDataType;
	
	class Json {
	public:
		explicit Json(){}
		explicit Json(std::string _jsonString) :jsonString(_jsonString) {
			std::list<char> jsonlist;
			for (auto c : jsonString)
				jsonlist.push_back(c);
			parseList(jsonlist);
		}

		int parse(std::string _jsonString) {
			if (!jsonData.empty())
				jsonData.clear();
			std::list<char> jsonlist;
			for (auto c : _jsonString)
				jsonlist.push_back(c);
			return parseList(jsonlist);
		}

		std::string stringify() {
			std::string res = "";
			if (jsonData.empty())
				return res;
			res += '{';
			unsigned int inde = 1;  //缩进数量
			stringifyObject(res, std::make_shared<jsonDataType>(jsonData), inde);
			res += '\n';
			res += '}';
			return res;
		}

		jsonStruct operator[](std::string name) {
			jsonStruct res;
			auto it = jsonData.find(name);
			if (it == jsonData.end()) {
				res.type = jsonWrongName;
				return res;
			}
			res = it->second;
			return res;
		}

	private:
		int parseList(std::list<char> &parseList) {
			int res= parseListCore(parseList, jsonData);
			if (res != PARSE_OK)
				return res;
			if (!parseList.empty())
				return LEFT_CONTENT;
			return res;
		}

		int parseListCore(std::list<char> &parseList, jsonDataType &data) {
			if (parseList.empty())
				return MISSING_FRONT_BRACKET;
			whitespaceParse(parseList);
			if (*parseList.begin() != '{')
				return MISSING_FRONT_BRACKET;
			do {
				parseList.pop_front(); //pop "," "{"
				if (*parseList.begin() == '}') {  //空对象
					parseList.pop_front();
					return PARSE_OK;
				}
				std::string name;
				int quit = parseName(parseList, name);
				if (quit != PARSE_OK)
					return quit;
				whitespaceParse(parseList);
				if (*parseList.begin() != ':')
					return MISSING_COLON;
				parseList.pop_front();
				jsonStruct value;
				quit = parseValue(parseList, value);
				if (quit != PARSE_OK)
					return quit;
				data.insert(make_pair(name, value));
				if (!parseList.empty())
					whitespaceParse(parseList);
				else
					return MISSING_BACK_BRACKET;
			} while (*parseList.begin() == ',');
			if (*parseList.begin() != '}')
				return MISSING_BACK_BRACKET;
			parseList.pop_front();
			return PARSE_OK;
		}
		
		int parseValue(std::list<char> &parseList, jsonStruct &value) {
			whitespaceParse(parseList);
			if (parseList.empty())
				return MISSING_QUOTATION;
			switch (*parseList.begin())
			{
			case  'n':return parseNull(parseList, value);
			case  't':return parseTrue(parseList, value);
			case  'f':return parseFalse(parseList, value);
			case  '{':return parseObject(parseList, value);
			case  '[':return parseArray(parseList, value);
			case  '\"':return parseString(parseList, value);
			default:
				return parseNumber(parseList, value);
			}
			return WRONG_VALUE;
		}

		bool isDigit(char c) {
			return (c >= '0'&&c <= '9') ? true : false;
		}

		int parseNumber(std::list<char> &parseList, jsonStruct &value) {
			std::string	num = "";
			if (parseList.empty())
				return WRONG_NUMBER;
			if (*parseList.begin() == '-') {
				num.append(1,*parseList.begin());
				parseList.pop_front();
			}
			if (parseList.empty())
				return WRONG_NUMBER;
			if (*parseList.begin() == '0') {
				num.append(1, *parseList.begin());
				parseList.pop_front();
			}
			else {
				if (parseList.empty())
					return WRONG_NUMBER;
				if (!(*parseList.begin() >= '1'&&*parseList.begin() <= '9'))
					return WRONG_NUMBER;
				if (parseList.empty())
					return WRONG_NUMBER;
				while (isDigit(*parseList.begin())) {
					num.append(1, *parseList.begin());
					parseList.pop_front();
					if (parseList.empty())
						return WRONG_NUMBER;
				}
			}
			if (parseList.empty())
				return WRONG_NUMBER;
			if (*parseList.begin() == '.') {
				num.append(1, *parseList.begin());
				parseList.pop_front();
				if (parseList.empty())
					return WRONG_NUMBER;
				while (isDigit(*parseList.begin())) {
					num.append(1, *parseList.begin());
					parseList.pop_front();
					if (parseList.empty())
						return WRONG_NUMBER;
				}
			}
			if (parseList.empty())
				return WRONG_NUMBER;
			if (*parseList.begin() == 'e' || *parseList.begin() == 'E') {
				num.append(1, *parseList.begin());
				parseList.pop_front();
				if (parseList.empty())
					return WRONG_NUMBER;
				if (*parseList.begin() == '+' || *parseList.begin() == '-') {
					num.append(1, *parseList.begin());
					parseList.pop_front();
				}
				if (parseList.empty())
					return WRONG_NUMBER;
				if (!isDigit(*parseList.begin()))
					return WRONG_NUMBER;
				while (isDigit(*parseList.begin())) {
					num.append(1, *parseList.begin());
					parseList.pop_front();
					if (parseList.empty())
						return WRONG_NUMBER;
				}
			}
			if (parseList.empty())
				return WRONG_NUMBER;
			whitespaceParse(parseList);
			if (*parseList.begin() != ',' && *parseList.begin() != ']' && *parseList.begin() != '}')
				return WRONG_NUMBER;
			std::stringstream ss;
			ss << num;
			ss >> value.number;
			value.type = jsonNumberType;
			return PARSE_OK;
		}

		int hex4toInt(std::list<char> &parseList, unsigned int &codeint) {   //  /uXXXX to int
			for (int i=0;i<4;i++)
			{
				if (parseList.empty())
					return MISSING_QUOTATION;
				char ch = *parseList.begin();
				parseList.pop_front();
				codeint <<= 4;
				if (ch >= '0' && ch <= '9')  codeint |= ch - '0';
				else if (ch >= 'A' && ch <= 'F')  codeint |= ch - 'A' + 10;
				else if (ch >= 'a' && ch <= 'f')  codeint |= ch - 'a' + 10;
				else return INVALID_UNICODE_HEX;
			}
			return PARSE_OK;
		}

		void encodeUTF8(std::string &res, unsigned int codeint) {    //   unicode to UTF-8
			if (codeint <= 0x7F)
				res += codeint & 0xFF;
			else if (codeint <= 0x7FF) {
				res += 0xC0 | ((codeint >> 6) & 0xFF);
				res += 0x80 | (codeint & 0x3F);
			}
			else if (codeint <= 0xFFFF) {
				res += 0xE0 | ((codeint >> 12) & 0xFF);
				res += 0x80 | ((codeint >> 6) & 0x3F);
				res += 0x80 | (codeint & 0x3F);
			}
			else {
				//assert(codeint <= 0x10FFFF);
				res += 0xF0 | ((codeint >> 18) & 0xFF);
				res += 0x80 | ((codeint >> 12) & 0x3F);
				res += 0x80 | ((codeint >> 6) & 0x3F);
				res += 0x80 | (codeint & 0x3F);
			}
		}

		int parseString(std::list<char> &parseList, jsonStruct &value) {
			parseList.pop_front();
			std::string res = "";
			if (parseList.empty())
				return MISSING_QUOTATION;
			while (*parseList.begin()!='\"'){
				if (*parseList.begin()=='\\')
				{
					parseList.pop_front();
					if (parseList.empty())
						return MISSING_QUOTATION;
					int quit = 0;
					unsigned int codeint = 0;
					switch (*parseList.begin())
					{
					case '\"':res += '\"'; parseList.pop_front(); break;
					case '\\':res += '\\'; parseList.pop_front(); break;
					case '/':res += '/'; parseList.pop_front(); break;
					case 'b':res += '\b'; parseList.pop_front(); break;
					case 'f':res += '\f'; parseList.pop_front(); break;
					case 'n':res += '\n'; parseList.pop_front(); break;
					case 'r':res += '\r'; parseList.pop_front(); break;
					case 't':res += '\t'; parseList.pop_front(); break;
					case 'u':
						parseList.pop_front();
						if ((quit = hex4toInt(parseList, codeint)) != PARSE_OK)
							return quit;
						if (codeint >= 0xD800 && codeint <= 0xDBFF) {
							if (parseList.empty())
								return MISSING_QUOTATION;
							if (*parseList.begin() != '\\')
								return INVALID_UNICODE;
							parseList.pop_front();
							if (parseList.empty())
								return MISSING_QUOTATION;
							if (*parseList.begin() != 'u')
								return INVALID_UNICODE;
							parseList.pop_front();
							unsigned int codeint2 = 0;
							if ((quit = hex4toInt(parseList, codeint2)) != PARSE_OK)
								return quit;
							if (codeint2 < 0xDC00 || codeint2 > 0xDFFF)
								return INVALID_UNICODE;
							codeint = (((codeint - 0xD800) << 10) | (codeint2 - 0xDC00)) + 0x10000;
						}
						encodeUTF8(res, codeint);
						break;
					default:return INVALID_ESC; break;
					}
				}
				if (parseList.empty())
					return MISSING_QUOTATION;
				if (*parseList.begin() < 0x20)
					return INVALID_STRING_CHAR;
				if (*parseList.begin() != '\"')
				{
					res += *parseList.begin();
					parseList.pop_front();
					if (parseList.empty())
						return MISSING_QUOTATION;
				}
			}
			parseList.pop_front();
			value.type = jsonStringType;
			value.string = std::make_shared<std::string>(res);
			return PARSE_OK;
		}

		int parseArray(std::list<char> &parseList, jsonStruct &value) {
			if (*parseList.begin() != '[')
				return MISSING_FRONT_BRACKET;
			std::vector<jsonStruct> vec;
			do 
			{
				parseList.pop_front();
				if (*parseList.begin() == ']') {  //空Array
					parseList.pop_front();
					value.type = jsonArrayType;
					value.array = std::make_shared<std::vector<jsonStruct>>(vec);
					return PARSE_OK;
				}
				jsonStruct data;
				whitespaceParse(parseList);
				int res= parseValue(parseList, data);
				whitespaceParse(parseList);
				if (res != PARSE_OK)
					return res;
				vec.push_back(data);
			} while (*parseList.begin()==',');
			value.array = std::make_shared<std::vector<jsonStruct>>(vec);
			whitespaceParse(parseList);
			if (*parseList.begin() != ']')
				return MISSING_BACK_BRACKET;
			parseList.pop_front();
			value.type = jsonArrayType;
			return PARSE_OK;
		}

		int parseObject(std::list<char> &parseList, jsonStruct &value) {
			jsonDataType data;
			int stat = parseListCore(parseList, data);
			value.obejct = std::make_shared<jsonDataType>(data);
			value.type = jsonObjectType;
			return stat;
		}

		int parseFalse(std::list<char> &parseList, jsonStruct &value) {
			auto iter = parseList.begin();
			if (parseList.size() >= 5) {
				if (*iter == 'f'&&*(++iter) == 'a'&&*(++iter) == 'l'&&*(++iter) == 's'&&*(++iter) == 'e')
				{
					parseList.erase(parseList.begin(), ++iter);
					value.type = jsonFalseType;
					return PARSE_OK;
				}
			}
			return WRONG_VALUE;
		}

		int parseTrue(std::list<char> &parseList, jsonStruct &value) {
			auto iter = parseList.begin();
			if (parseList.size() >= 4) {
				if (*iter == 't'&&*(++iter) == 'r'&&*(++iter) == 'u'&&*(++iter) == 'e')
				{
					parseList.erase(parseList.begin(), ++iter);
					value.type = jsonTrueType;
					return PARSE_OK;
				}
			}
			return WRONG_VALUE;
		}

		int parseNull(std::list<char> &parseList, jsonStruct &value) {
			auto iter = parseList.begin();
			if (parseList.size() >= 4) {
				if (*iter == 'n'&&*(++iter) == 'u'&&*(++iter) == 'l'&&*(++iter) == 'l')
				{
					parseList.erase(parseList.begin(), ++iter);
					value.type = jsonNullType;
					return PARSE_OK;
				}
			}
			return WRONG_VALUE;
		}

		int parseName(std::list<char> &parseList,std::string &name) {
			whitespaceParse(parseList);
			if (*parseList.begin() != '"')
				return MISSING_QUOTATION;
			parseList.pop_front();
			if (parseList.empty())
				return MISSING_NAME;
			while (*parseList.begin()!='"'){
				name.push_back(parseList.front());
				parseList.pop_front();
				if (parseList.empty())
					return MISSING_QUOTATION;
			}
			if (name.empty())
				return MISSING_NAME;
			parseList.pop_front();
			return PARSE_OK;
		}

		void whitespaceParse(std::list<char> &parseList) {
			if (!parseList.empty())
			{
				while (*parseList.begin() == ' ' || *parseList.begin() == '\t' || \
					*parseList.begin() == '\n' || *parseList.begin() == '\r')
					parseList.pop_front();
			}
		}

		std::string stringWithInde(std::string content, unsigned int inde) {
			std::string res = "";
			for (unsigned int i = 0; i < inde; i++)
				res += '\t';
			res += content;
			return res;
		}

		void stringifyObject(std::string &res, std::shared_ptr<std::map<std::string, jsonStruct>> data, unsigned int inde) {
			std::stringstream ss;
			bool cnt = false;
			for (auto it : *data)
			{
				if (cnt) res += ',';
				cnt = true;
				res += '\n';
				res += stringWithInde(it.first + ':', inde);
				switch (it.second.getType())
				{
				case jsonNullType:res += "null"; break;
				case jsonTrueType:res += "true"; break;
				case jsonFalseType:res += "false"; break;
				case jsonNumberType:
					ss.str("");   //ss.clear()是清空ss状态而不是内容
					ss << it.second.getNumber();
					res += ss.str();
					break;
				case jsonStringType:res += it.second.getString(); break;
				case jsonArrayType:
					res += '[';
					stringifyArray(res, it.second.getArray(), inde + 1);
					res += '\n';
					res += stringWithInde("]", inde); break;
				case jsonObjectType:
					res += '{';
					stringifyObject(res, it.second.getObject(), inde + 1);
					res += '\n';
					res += stringWithInde("}", inde); break;
				default:
					break;
				}
			}
		}

		void stringifyArray(std::string &res, std::shared_ptr<std::vector<jsonStruct>> array, unsigned int inde) {
			std::stringstream ss;
			bool cnt = false;
			for (auto it : *array)
			{
				if (cnt) res += ',';
				cnt = true;
				res += '\n';
				switch (it.getType())
				{
				case jsonNullType:res += stringWithInde("null", inde); break;
				case jsonTrueType:res += stringWithInde("true", inde); break;
				case jsonFalseType:res += stringWithInde("false", inde); break;
				case jsonNumberType:
					ss.str("");
					ss << it.getNumber();
					res += stringWithInde(ss.str(), inde);
					break;
				case jsonStringType:res += stringWithInde(it.getString(), inde); break;
				case jsonArrayType:
					res += stringWithInde("[", inde);
					stringifyArray(res, it.getArray(), inde + 1);
					res += '\n';
					res += stringWithInde("]", inde); break;
				case jsonObjectType:
					res += stringWithInde("{", inde);
					stringifyObject(res, it.getObject(), inde + 1);
					res += '\n';
					res += stringWithInde("}", inde); break;
				default:
					break;
				}
			}
		}

		std::string jsonString;
		jsonDataType jsonData;
	};
}