#include "gtest/gtest.h"
#include "json.hpp"

using namespace JSON;

TEST(typetest, null_true_false_test){
	Json a = Json("{\"sd\":null}");
	EXPECT_EQ(jsonNullType,a["sd"].getType());
	Json b = Json("{\"ad\":null,\"aa\":true,\"1\":false,\"2\":12,\"3\":{},\"s2\":[1,false,\"sss\",4],\"s3\":[]}");
	EXPECT_EQ(jsonNullType, b["ad"].getType());
	EXPECT_EQ(jsonTrueType, b["aa"].getType());
	EXPECT_EQ(jsonFalseType, b["1"].getType());
	EXPECT_EQ(jsonNumberType, b["2"].getType());
	EXPECT_EQ(jsonObjectType, b["3"].getType());
	EXPECT_EQ(jsonArrayType, b["s2"].getType());
	EXPECT_EQ(jsonArrayType, b["s3"].getType());

	Json c;
	EXPECT_EQ(MISSING_QUOTATION, c.parse("{\"as:true}"));
	EXPECT_EQ(MISSING_QUOTATION, c.parse("{\"dd\":true,,\"sss\":false}"));
	EXPECT_EQ(MISSING_BACK_BRACKET, c.parse("{\"0\":false,\"sssssssssssssssssssssssss\":true"));
	EXPECT_EQ(MISSING_FRONT_BRACKET, c.parse(""));
	EXPECT_EQ(MISSING_COLON, c.parse("{\".1\"true}"));
	EXPECT_EQ(WRONG_VALUE, c.parse("{\"sd\":truE}"));
	EXPECT_EQ(WRONG_VALUE, c.parse("{\"sd\":nul}"));
	EXPECT_EQ(WRONG_VALUE, c.parse("{\"sd\":f"));
	EXPECT_EQ(PARSE_OK, c.parse("{\"sss\":false}"));
	EXPECT_EQ(PARSE_OK, c.parse("{\"sss\":true}"));
	EXPECT_EQ(PARSE_OK, c.parse("{\"sss\":false}"));
}

TEST(objecttest, test1) {
	Json a;
	a.parse("{\"aa\":{\"aob\":2.34567e2},\"sss\":false}");
	EXPECT_EQ(jsonObjectType, a["aa"].getType());
	EXPECT_EQ(jsonFalseType, a["sss"].getType());
	JSON::jsonDataType sdata;
	jsonStruct test;
	test = a["sss"];
	jsonStruct t2;
	t2 = a["aa"];
	EXPECT_EQ(true, test.isFalse());
	EXPECT_EQ(true, t2.isObject());
	EXPECT_EQ(2.34567e2, t2["aob"].getNumber());
}

TEST(stringtest, test1) {
	Json a;
	EXPECT_EQ(MISSING_QUOTATION, a.parse("{\"s\":\"a34234a}"));
	EXPECT_EQ(PARSE_OK, a.parse("{\"x\":\"1wewewfdskfcjnscbjxbc\",\"1\":\"Hello\\nWorld\",\"2\":\"\\\" \\\\ \\/ \\b \\f \\n \\r \\t\",\"3\":\"Hello\\u0001World\"}"));
	EXPECT_EQ("1wewewfdskfcjnscbjxbc", a["x"].getString());
	EXPECT_EQ("Hello\nWorld", a["1"].getString());
	EXPECT_EQ("\" \\ / \b \f \n \r \t", a["2"].getString());
	EXPECT_EQ("Hello\1World", a["3"].getString());
	EXPECT_EQ(PARSE_OK, a.parse("{\"1\":\"\\u0024\",\"2\":\"\\u00A2\",\"3\":\"\\u20AC\",\"4\":\"\\uD834\\uDD1E\",\"5\":\"\\ud834\\udd1e\"}"));
	EXPECT_EQ("\x24", a["1"].getString());
	EXPECT_EQ("\xC2\xA2", a["2"].getString());
	EXPECT_EQ("\xE2\x82\xAC", a["3"].getString());
	EXPECT_EQ("\xF0\x9D\x84\x9E", a["4"].getString());
	EXPECT_EQ("\xF0\x9D\x84\x9E", a["5"].getString());
	
	EXPECT_EQ(INVALID_ESC, a.parse("{\"s\":\"\\v\"}"));
	EXPECT_EQ(INVALID_ESC, a.parse("{\"s\":\"\\'\"}"));
	EXPECT_EQ(INVALID_ESC, a.parse("{\"s\":\"\\0\"}"));
	EXPECT_EQ(INVALID_ESC, a.parse("{\"s\":\"\\x12\"}"));
	
	EXPECT_EQ(INVALID_STRING_CHAR, a.parse("{\"s\":\"\x01\"}"));
	EXPECT_EQ(INVALID_STRING_CHAR, a.parse("{\"s\":\"\x1F\"}"));

	EXPECT_EQ(INVALID_UNICODE_HEX, a.parse("{\"s\":\"a34\\ua12#234a\"}}"));
	EXPECT_EQ(INVALID_UNICODE_HEX, a.parse("{\"s\":\"\\u\"}"));
	EXPECT_EQ(INVALID_UNICODE_HEX, a.parse("{\"s\":\"\\u0\"}"));
	EXPECT_EQ(INVALID_UNICODE_HEX, a.parse("{\"s\":\"\\u01\"}"));
	EXPECT_EQ(INVALID_UNICODE_HEX, a.parse("{\"s\":\"\\u012\"}"));
	EXPECT_EQ(INVALID_UNICODE_HEX, a.parse("{\"s\":\"\\u/000\"}"));
	EXPECT_EQ(INVALID_UNICODE_HEX, a.parse("{\"s\":\"\\uG000\"}"));
	EXPECT_EQ(INVALID_UNICODE_HEX, a.parse("{\"s\":\"\\u0/00\"}"));
	EXPECT_EQ(INVALID_UNICODE_HEX, a.parse("{\"s\":\"\\u0G00\"}"));
	EXPECT_EQ(INVALID_UNICODE_HEX, a.parse("{\"s\":\"\\u0/00\"}"));
	EXPECT_EQ(INVALID_UNICODE_HEX, a.parse("{\"s\":\"\\u00G0\"}"));
	EXPECT_EQ(INVALID_UNICODE_HEX, a.parse("{\"s\":\"\\u000/\"}"));
	EXPECT_EQ(INVALID_UNICODE_HEX, a.parse("{\"s\":\"\\u000G\"}"));
	EXPECT_EQ(INVALID_UNICODE_HEX, a.parse("{\"s\":\"\\u 123\"}"));

	EXPECT_EQ(INVALID_UNICODE, a.parse("{\"s\":\"\\uD800\"}"));
	EXPECT_EQ(INVALID_UNICODE, a.parse("{\"s\":\"\\uDBFF\"}"));
	EXPECT_EQ(INVALID_UNICODE, a.parse("{\"s\":\"\\uD800\\\\\"}"));
	EXPECT_EQ(INVALID_UNICODE, a.parse("{\"s\":\"\\uD800\\uDBFF\"}"));
	EXPECT_EQ(INVALID_UNICODE, a.parse("{\"s\":\"\\uD800\\uE000\"}"));
}

TEST(numbertest, test1) {
	Json a;
	a.parse("{\"s\":-1.234e5 ,  \"a\" :   0.0000000000000000000000000001  }");
	EXPECT_EQ(-123400.0f, a["s"].getNumber());
	EXPECT_EQ(1.0e-28, a["a"].getNumber());
	EXPECT_EQ(WRONG_NUMBER, a.parse("{\"p\":10e-}"));
	EXPECT_EQ(WRONG_NUMBER, a.parse("{\"p\":00}"));
	EXPECT_EQ(WRONG_NUMBER, a.parse("{\"p\":+0.4}"));
	EXPECT_EQ(WRONG_NUMBER, a.parse("{\"p\":-01232323.213-213213}"));
	EXPECT_EQ(WRONG_NUMBER, a.parse("{\"p\":00.3324}"));
	EXPECT_EQ(WRONG_NUMBER, a.parse("{\"p\":56790.000e}"));
	EXPECT_EQ(WRONG_NUMBER, a.parse("{\"p\":0.3234e0.5}"));
	EXPECT_EQ(WRONG_NUMBER, a.parse("{\"p\":100e+}"));
	EXPECT_EQ(WRONG_NUMBER, a.parse("{\"p\":2.oE+2}"));
}

TEST(arraytest, test1) {
	Json a;
	EXPECT_EQ(PARSE_OK,a.parse("{ \"s\":[true,true,false,null,\"hi\",true,{ \"s\":[1,2,3,4] },[[[12]]]] }"));
	EXPECT_EQ(true, a["s"][1].isTrue());
	EXPECT_EQ(true, a["s"][3].isNull());
	EXPECT_EQ(true, a["s"][6].isObject());
	EXPECT_EQ(1.f, a["s"][6]["s"][0].getNumber());
}

TEST(stringifytest, test1) {
	Json b;
	EXPECT_EQ(PARSE_OK, b.parse("{ \"s\":true, \"s1\":true, \"s2\":[2,false,[[1],2],\"sss\",{ \"s\":[1,2,3,4] }], \"s3\":{ \"s\":[1,2,3,4] }, \"s4\":\"hi\", \"s5\":12.03434323e-23 }"));
	EXPECT_EQ("{\n\ts:true,\n\ts1:true,\n\ts2:[\n\t\t2,\n\t\tfalse,\n\t\t[\n\t\t\t[\n\t\t\t\t1\n\t\t\t],\n\t\t\t2\n\t\t],\n\t\tsss,\n\t\t{\n\t\t\ts:[\n\t\t\t\t1,\n\t\t\t\t2,\n\t\t\t\t3,\n\t\t\t\t4\n\t\t\t]\n\t\t}\n\t],\n\ts3:{\n\t\ts:[\n\t\t\t1,\n\t\t\t2,\n\t\t\t3,\n\t\t\t4\n\t\t]\n\t},\n\ts4:hi,\n\ts5:1.20343e-22\n}", b.stringify());
	EXPECT_EQ(PARSE_OK, b.parse("{\"sadas\":{},\"aa\":[]}"));
	EXPECT_EQ("{\n\taa:[\n\t],\n\tsadas:{\n\t}\n}", b.stringify());
}

int main(int argc, char* argv[]) {
	testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}























