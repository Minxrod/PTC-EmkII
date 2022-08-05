#include "catch.hpp"

#include "test_util.hpp"

#include "Tokens.hpp"

TEST_CASE("Tokenize (GRPED:127)", "tokenize"){
	auto tokens = tokenize_str("K=B AND (NOT O)\r");
	
	REQUIRE(tokens.size() == 9);
	
	REQUIRE(tokens[0] == Token{L"K", Type::Var});
	REQUIRE(tokens[1] == Token{L"=", Type::Op});
	REQUIRE(tokens[2] == Token{L"B", Type::Var});
	REQUIRE(tokens[3] == Token{L"AND", Type::Op});
	REQUIRE(tokens[4] == Token{L"(", Type::Op});
	REQUIRE(tokens[5] == Token{L"NOT", Type::Op});
	REQUIRE(tokens[6] == Token{L"O", Type::Var});
	REQUIRE(tokens[7] == Token{L")", Type::Op});
	REQUIRE(tokens[8] == Token{L"\r", Type::Newl});
}

TEST_CASE("Tokenize (NOT O)", "tokenize"){
	auto tokens = tokenize_str("NOT O");
	
	REQUIRE(tokens.size() == 2);
	
	REQUIRE(tokens[0] == Token{L"NOT", Type::Op});
	REQUIRE(tokens[1] == Token{L"O", Type::Var});
}

TEST_CASE("Tokenize (MM2PTC:2711)", "tokenize"){
	auto tokens = tokenize_str("DCMPSIZE=3000:DCMPS=0OR(DCMPSIZE/256)+!!(DCMPSIZE%256)");
	
	REQUIRE(tokens.size() == 21);
	
	REQUIRE(tokens[0] == Token{L"DCMPSIZE", Type::Var});
	REQUIRE(tokens[1] == Token{L"=", Type::Op});
	REQUIRE(tokens[2] == Token{L"3000", Type::Num});
	REQUIRE(tokens[3] == Token{L":", Type::Newl});
	REQUIRE(tokens[4] == Token{L"DCMPS", Type::Var});
	REQUIRE(tokens[5] == Token{L"=", Type::Op});
	REQUIRE(tokens[6] == Token{L"0", Type::Num});
	REQUIRE(tokens[7] == Token{L"OR", Type::Op});
	REQUIRE(tokens[8] == Token{L"(", Type::Op});
	REQUIRE(tokens[9] == Token{L"DCMPSIZE", Type::Var});
	REQUIRE(tokens[10] == Token{L"/", Type::Op});
	REQUIRE(tokens[11] == Token{L"256", Type::Num});
	REQUIRE(tokens[12] == Token{L")", Type::Op});
	REQUIRE(tokens[13] == Token{L"+", Type::Op});
	REQUIRE(tokens[14] == Token{L"!", Type::Op});
	REQUIRE(tokens[15] == Token{L"!", Type::Op});
	REQUIRE(tokens[16] == Token{L"(", Type::Op});
	REQUIRE(tokens[17] == Token{L"DCMPSIZE", Type::Var});
	REQUIRE(tokens[18] == Token{L"%", Type::Op});
	REQUIRE(tokens[19] == Token{L"256", Type::Num});
	REQUIRE(tokens[20] == Token{L")", Type::Op});
}
