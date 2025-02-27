#include "argparse.hpp"
#include "fmt/core.h"

#include "tokenizer/tokenizer.h"
#include "analyser/analyser.h"
#include "fmts.hpp"

#include <cstdint>
#include <iostream>
#include <fstream>

std::vector<cc0::Token> _tokenize(std::istream& input) {
	cc0::Tokenizer tkz(input);
	auto p = tkz.AllTokens();
	// error in tokenizer
	if (p.second.has_value()) {
		fmt::print(stderr, "Tokenization error: {}\n", p.second.value());
		exit(2);
	}
	return p.first;
}

void Tokenize(std::istream& input, std::ostream& output) {
	auto v = _tokenize(input);
	for (auto& it : v) {
		output << fmt::format("{}\n", it);
	}
}

void ToAssembly(std::istream& input, std::ostream& output) {
	// tokenizer
	auto tks = _tokenize(input);
	// analyser
	cc0::Analyser analyser(tks);
	auto p = analyser.Analyse();
	// error in analyser
	if (p.second.has_value()) {
		fmt::print(stderr, "Syntactic analysis error: {}\n", p.second.value());
		exit(2);
	}
	// output constant table
	auto consts = analyser.getConstants();
	int const_size = consts.size();
	output << ".constants:" << std::endl;
	for (int i = 0; i < const_size; i++) {
		// 三元式：下标|常量的类型|常量的值
		output << i << " S \"" << consts[i].getName() << "\"" << std::endl;
	}

	// output start_code
	auto v = p.first;
	output << ".start:" << std::endl;
	int size = v[-1].size();
	for (int i = 0; i < size; i++) {
		output << fmt::format("{}   {}\n", i, v[-1][i]);
	}

	// output constant table
	output << ".functions:" << std::endl;
	int funcIndex = 0;
	for (int i = 0; i < const_size; i++) {
		if (consts[i].isFunction())
			// 四元式：下标|函数名在.constants中的下标|参数占用的slot数|函数嵌套的层级
			output << funcIndex++ << " " << i << " " << consts[i].getParamNum() << " 1" << std::endl;
	}

	funcIndex = 0;
	for (int i = 0; i < const_size; i++) {
		if (consts[i].isFunction()) {
			output << ".F" << funcIndex << ":" << std::endl;
			funcIndex++;
			int size = v[i].size();
			for (int j = 0; j < size; j++) {
				output << fmt::format("{}   {}\n", j, v[i][j]);
			}
		}
	}
}

void writeBytes(void* addr, int count, std::ostream& out) {
	char bytes[8];
	assert(0 < count && count <= 8);
	char* p = reinterpret_cast<char*>(addr) + (count - 1);
	for (int i = 0; i < count; i++) {
		bytes[i] = *p--;
	}
	out.write(bytes, count);
}

void ToBinary(std::istream& input, std::ostream& out) {
	auto tks = _tokenize(input);
	cc0::Analyser analyser(tks);
	auto p = analyser.Analyse();
	if (p.second.has_value()) {
		fmt::print(stderr, "Syntactic analysis error: {}\n", p.second.value());
		exit(2);
	}
	// get constant table
	auto consts = analyser.getConstants();

	// output magic must be 0x43303A29
	out.write("\x43\x30\x3A\x29", 4);
	// output version 1
	out.write("\x00\x00\x00\x01", 4);

	// constants_count
	std::uint16_t constants_count = consts.size();
	writeBytes(&constants_count, sizeof(constants_count), out);
	// constants
	for (auto& constant : consts) {
		// const string
		if (constant.isFunction() || constant.getType() == cc0::SymType::STRING_TYPE) {
			out.write("\x00", 1);
			std::string str = constant.getName();
			std::uint16_t len = str.length();
			// output str_length
			writeBytes(&len, sizeof(len), out);
			// output str_content
			out.write(str.c_str(), len);
		}
		else if (constant.getType() == cc0::SymType::INT_TYPE) {
			out.write("\x01", 1);
		}
		else if (constant.getType() == cc0::SymType::DOUBLE_TYPE) {
			out.write("\x02", 1);
		}
	}

	auto to_binary = [&](const std::vector<cc0::Instruction>& v) {
		// std::uint16_t instructions_count;
		std::uint16_t intro_size = v.size();
		writeBytes(&intro_size, sizeof(intro_size), out);
		// Instruction instructions[instructions_count];
		for (auto& intro : v) {
			// output instructiuon
			std::uint8_t opt = static_cast<std::uint8_t>(intro.getOperation());
			writeBytes(&opt, sizeof(opt), out);
			// if there are vars after instruction
			auto iter = cc0::paramOpt.find(intro.getOperation());
			if (iter != cc0::paramOpt.end()) {
				auto params = iter->second;
				switch (params[0]) {
				case 1: {
					std::uint8_t x = intro.getX();
					writeBytes(&x, 1, out);
					break;
				}
				case 2: {
					std::uint16_t x = intro.getX();
					writeBytes(&x, 2, out);
					break;
				}
				case 4: {
					std::uint32_t x = intro.getX();
					writeBytes(&x, 4, out);
					break;
				}
				default:
					break;
				}
				if (params.size() == 2) {
					switch (params[1]) {
					case 1: {
						std::uint8_t y = intro.getY();
						writeBytes(&y, 1, out);
						break;
					}
					case 2: {
						std::uint16_t y = intro.getY();
						writeBytes(&y, 2, out);
						break;
					}
					case 4: {
						std::uint32_t y = intro.getY();
						writeBytes(&y, 4, out);
						break;
					}
					default:
						break;
					}
				}
			}
		}
	};

	// all instructions
	auto introductions_code = p.first;
	// start_code
	auto start_code = introductions_code[-1];
	to_binary(start_code);

	// functions_count
	std::uint16_t functions_count = introductions_code.size() - 1;
	writeBytes(&functions_count, sizeof(functions_count), out);

	// functions
	for (int i = 0; i < constants_count; i++) {
		// 注意常量和函数是放在一起的
		if (consts[i].isFunction()) {
			// std::uint16_t name_index;
			// name: CO_binary_file.strings[name_index]
			std::uint16_t funcIndex = i;
			writeBytes(&funcIndex, sizeof(funcIndex), out);
			// std::uint16_t params_size;
			std::uint16_t paramSize = consts[i].getParamNum();
			writeBytes(&paramSize, sizeof(paramSize), out);
			// std::uint16_t level;
			std::uint16_t level = 1;
			writeBytes(&level, sizeof(level), out);
			to_binary(introductions_code[i]);
		}
	}
}

//program entrance
int main(int argc, char** argv) {
	argparse::ArgumentParser program("cc0");
	program.add_argument("input")
		.help("speicify the file to be compiled.");

	program.add_argument("-s")
		.default_value(false)
		.implicit_value(true)
		.help("Translate the input c0-source code into a text assembly file.");

	program.add_argument("-c")
		.default_value(false)
		.implicit_value(true)
		.help("Translate the input c0-source code into binary target file.");
	program.add_argument("-o", "--output")
		.required()
		.default_value(std::string("-"))
		.help("specify the output file.");

	try {
		program.parse_args(argc, argv);
	}
	catch (const std::runtime_error& err) {
		fmt::print(stderr, "{}\n\n", err.what());
		program.print_help();
		exit(2);
	}

	auto input_file = program.get<std::string>("input");
	auto output_file = program.get<std::string>("--output");
	std::istream* input;
	std::ostream* output;
	std::ifstream inf;
	std::ofstream outf;
	if (input_file != "-") {
		inf.open(input_file, std::ios::in);
		if (!inf) {
			fmt::print(stderr, "Fail to open {} for reading.\n", input_file);
			exit(2);
		}
		input = &inf;
	}
	else {
		input = &std::cin;
	}


	if (program["-s"] == true && program["-c"] == true) {
		fmt::print(stderr, "You can only perform -s or -c at one time.");
		exit(2);
	}
	if (program["-s"] == true) {
		if (output_file != "-") {
			outf.open(output_file, std::ios::out | std::ios::trunc);
			if (!outf) {
				fmt::print(stderr, "Fail to open {} for writing.\n", output_file);
				exit(2);
			}
			output = &outf;
		}
		else {
			outf.open("out", std::ios::out | std::ios::trunc);
			if (!outf) {
				fmt::print(stderr, "Fail to open {} for writing.\n", output_file);
				exit(2);
			}
			output = &outf;
		}

		ToAssembly(*input, *output);

	}
	else if (program["-c"] == true) {
		if (output_file != "-") {
			outf.open(output_file, std::ios::out | std::ios::trunc);
			if (!outf) {
				fmt::print(stderr, "Fail to open {} for writing.\n", output_file);
				exit(2);
			}
			output = &outf;
		}
		else {
			outf.open("out", std::ios::out | std::ios::trunc);
			if (!outf) {
				fmt::print(stderr, "Fail to open {} for writing.\n", output_file);
				exit(2);
			}
			output = &outf;
		}

		ToBinary(*input, *output);
	}
	else {
		fmt::print(stderr, "You must choose -s or -c type.");
		exit(2);
	}
	return 0;
}