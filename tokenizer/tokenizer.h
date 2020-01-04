#pragma once

#include "tokenizer/token.h"
#include "tokenizer/utils.hpp"
#include "error/error.h"

#include <utility>
#include <optional>
#include <iostream>
#include <cstdint>
#include <memory>
#include <vector>
#include <string>
#include <map>

namespace cc0 {

	class Tokenizer final {
	private:
		using uint64_t = std::uint64_t;

		enum DFAState {
			INITIAL_STATE, // init
			ZERO_STATE, // int
			IDENTIFIER_STATE, // id
			DECIMAL_INTEGER_STATE, // 10
			HEXADECIMAL_INTEGER_STATE, // 16
			CHAR_STATE,  // char
			STRING_STATE, // string
			PLUS_SIGN_STATE,  // +
			MINUS_SIGN_STATE,  // -
			MULTIPLICATION_SIGN_STATE,  // *
			DIVISION_SIGN_STATE,  // /
			EQUAL_SIGN_STATE,   // =
			LEFTBRACKET_STATE,  // (
			RIGHTBRACKET_STATE,  // )
			LEFT_BRACE_STATE,  // {
			RIGHT_BRACE_STATE,  // }
			LESS_SIGN_STATE,  // <
			GREATER_SIGN_STATE,  //  >
            COMMA_SIGN_STATE,  // ,
			SEMICOLON_STATE,    // ;
			
			ZERO_DIGIT_STATE, // 0+digit
			DIGIT_STATE, // .digit
			POINT_STATE, // .digit
			E_STATE, // e...
            EXCLAMATION_SIGN_STATE,  // !
            SINGLE_LINE_COMMENT_STATE, // //
            MULTI_LINE_COMMENT_STATE,  // /* */
		};
	public:
		Tokenizer(std::istream& ifs)
			: _rdr(ifs), _initialized(false), _ptr(0, 0),_lines_buffer() {}
		Tokenizer(Tokenizer&& tkz) = delete;
		Tokenizer(const Tokenizer&) = delete;
		Tokenizer& operator=(const Tokenizer&) = delete;

		// 核心函数，返回下一个 token
		std::pair<std::optional<Token>, std::optional<CompilationError>> NextToken();
		// 一次返回所有 token
		std::pair<std::vector<Token>, std::optional<CompilationError>> AllTokens();
	private:
		// 检查 Token 的合法性
		std::optional<CompilationError> checkToken(const Token&);

		// 返回下一个 token，是 NextToken 实际实现部分
		std::pair<std::optional<Token>, std::optional<CompilationError>> nextToken();

		// 基于行号的缓冲区的实现
		void readAll();

		// nextPos() = (1, 0)
		// currentPos() = (0, 9)
		// previousPos() = (0, 8)
		// nextChar() = '\n' 并且指针移动到 (1, 0)
		// unreadLast() 指针移动到 (0, 8)
		std::pair<uint64_t, uint64_t> nextPos();
		std::pair<uint64_t, uint64_t> currentPos();
		std::pair<uint64_t, uint64_t> previousPos();
		std::optional<char> nextChar();
		bool isEOF();
		void unreadLast();
		bool isHex(char ch);
		bool isChar(char ch);
		bool isEscape(std::stringstream& ss, char ch);
		
	private:
		std::istream& _rdr;
		// 如果没有初始化，那么就 readAll
		bool _initialized;
		// 指向下一个要读取的字符
		std::pair<uint64_t, uint64_t> _ptr;
		// 以行为基础的缓冲区
		std::vector<std::string> _lines_buffer;
    };
}
