#pragma once

#include <cstdint>
#include <unordered_map>
#include <vector>

namespace cc0 {

	enum Operation {
		//copy from co-vm-cpp:OpCode:u1
		// do nothing
		NOP = 0x00,

		// bipush value(1)
		// ...
		// ..., value
		BIPUSH = 0x01,

		// ipush value(4)
		// ...
		// ..., value
		IPUSH = 0x02,

		// pop / popn count(4)
		// ..., value(n)
		// ...
		POP = 0x04, POP2 = 0x05, POPN = 0x06,

		// dup
		// ..., value
		// ..., value, value
		DUP = 0x07, DUP2 = 0x08,

		// loadc index(2)
		// ...
		// ..., value
		LOADC = 0x09,

		// loada level_diff(2), offser(4)
		// ...
		// ..., addr
		LOADA = 0x0a,

		// new
		// ..., count
		// ..., addr
		NEW = 0x0b,
		// snew count(4)
		// ...
		// ..., value
		SNEW = 0x0c,

		// Tload
		// ..., addr
		// ..., value
		ILOAD = 0x10, DLOAD = 0x11, ALOAD = 0x12,
		// Taload
		// ..., array, index
		// ..., value
		IALOAD = 0x18, DALOAD = 0x19, AALOAD = 0x1a,
		// Tstore
		// ..., addr, value
		// ...
		ISTORE = 0x20, DSTORE = 0x21, ASTORE = 0x22,
		// Tastore
		// ..., array, index, value
		// ...
		IASTORE = 0x28, DASTORE = 0x29, AASTORE = 0x2a,

		// Tadd
		// ..., lhs, rhs
		// ..., result
		IADD = 0x30, DADD = 0x31,

		// Tsub
		// ..., lhs, rhs
		// ..., result
		ISUB = 0x34, DSUB = 0x35,

		// Tmul
		// ..., lhs, rhs
		// ..., result
		IMUL = 0x38, DMUL = 0x39,
		// Tdiv
		// ..., lhs, rhs
		// ..., result
		IDIV = 0x3c, DDIV = 0x3d,
		// Tneg
		// ..., value
		// ..., result
		INEG = 0x40, DNEG = 0x41,
		// Tcmp
		// ..., lhs, rhs
		// ..., result
		ICMP = 0x44, DCMP = 0x45,

		// T2T
		// ..., value
		// ..., result
		I2D = 0x60, D2I = 0x61, I2C = 0x62,

		// jmp offset(2)
		JMP = 0x70,

		// jCOND offset(2)
		// ..., value
		// ...
		JE = 0x71, JNE = 0x72, JL = 0x73, JGE = 0x74, JG = 0x75, JLE = 0x76,

		// call index(2)
		// ..., params
		// ...
		CALL = 0x80,

		// ret
		RET = 0x88,

		// Tret
		IRET = 0x89, DRET = 0x8a, ARET = 0x8b,

		// Tprint
		// ..., value
		// ...
		IPRINT = 0xa0, DPRINT = 0xa1, CPRINT = 0xa2, SPRINT = 0xa3,

		// printl
		PRINTL = 0xaf,

		// Tscan
		// ...
		// ..., value
		ISCAN = 0xb0, DSCAN = 0xb1, CSCAN = 0xb2,
	};

	const std::unordered_map<cc0::Operation, std::vector<int>> paramOpt = {
	        // bipush byte(1) (0x01)
            {Operation::BIPUSH, {1} },
            // ipush value(4) (0x02)
            {Operation::IPUSH, {4} },
            // popn count(4) (0x06)
            {Operation::POPN, {4} },
            // loadc index(2) (0x09)
            {Operation::LOADC, {2} },
            // loada level_diff(2), offset(4) (0x0a)
            {Operation::LOADA, {2, 4} },
            // snew count(4) (0x0c)
            {Operation::SNEW, {4} },
            // call index(2) (0x80)
            {Operation::CALL, {2} },
            // jmp offset(2) (0x70)
            {Operation::JMP, {2} },
            // je offset(2) (0x71)
            {Operation::JE, {2} },
            {Operation::JNE, {2} },
            {Operation::JL, {2} },
            {Operation::JLE, {2} },
            {Operation::JG, {2} },
            {Operation::JGE, {2} },
	};
	
	class Instruction final {
	private:
        using int32_t = std::int32_t;
	public:
		friend void swap(Instruction& lhs, Instruction& rhs);
	public:
	    Instruction(Operation opr) : _opr(opr) {}
	    Instruction(Operation opr, int32_t x) : _opr(opr), _x(x) {}
		Instruction(Operation opr, int32_t x,int32_t y) : _opr(opr), _x(x), _y(y) {}

		Instruction() = default;
		Instruction(const Instruction& i) { _opr = i._opr; _x = i._x; _y = i._y; }
		Instruction(Instruction&& i) :Instruction() { swap(*this, i); }
		Instruction& operator=(Instruction i) { swap(*this, i); return *this; }
		bool operator==(const Instruction& i) const { return _opr == i._opr && _x == i._x && _y == i._y; }

		Operation getOperation() const { return _opr; }
		int32_t getX() const { return _x; }
		int32_t getY() const { return _y; }
		void setX(int32_t x) { _x = x; }
	private:
		Operation _opr;
		int32_t _x = -1;
		int32_t _y = -1;
	};

	inline void swap(Instruction& lhs, Instruction& rhs) {
		using std::swap;
		swap(lhs._opr, rhs._opr);
		swap(lhs._x, rhs._x);
		swap(lhs._y, rhs._y);
	}
}