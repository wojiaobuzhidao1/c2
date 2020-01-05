#pragma once

#include <string>
#include <cstdint>
#include <utility>
#include <vector>

namespace cc0 {
	enum SymType {
		// func
		UNLIMITED,
		// func
		VOID_TYPE,
		// func | var
		INT_TYPE,
		// double s
		DOUBLE_TYPE,
		// char
		CHAR_TYPE,
		// string
		STRING_TYPE,
	};

	class Symbol final {
	private:
		using uint64_t = std::uint64_t;
		using int32_t = std::int32_t;

	public:
		Symbol(std::string name, bool isFunc, bool isConst, SymType type, int32_t index, int32_t param_num)
			:_name(std::move(name)), _isFunc(isFunc), _isConst(isConst), _type(type), _index(index), _param_num(param_num) {};
		Symbol(Symbol* t) { _name = t->_name; _isFunc = t->_isFunc; _isConst = t->_isConst; _type = t->_type; _index = t->_index; _param_num = t->_param_num; };

	public:
		std::string getName() { return _name; };
		int32_t getIndex() const { return _index; };
		int32_t getParamNum() const { return _param_num; };
		bool isFunction() const { return _isFunc; };
		SymType getType() const { return _type; };
		bool isConst() const { return _isConst; };
		bool isInit() const { return _isInit; };
		void initVar() { _isInit = true; };
		void setParamNum(int32_t param_num) { _param_num = param_num; };
	private:
		std::string _name;    // 标识符
		// 1 func 0 var
		bool _isFunc;
		bool _isConst;
		SymType _type;
		int32_t _index;
		// init
		bool _isInit = false;
		// num of para
		int32_t _param_num;
	};
}