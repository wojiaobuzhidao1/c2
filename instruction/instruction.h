#pragma once

#include <cstdint>
#include <unordered_map>
#include <vector>

namespace cc0 {

	enum Operation {
	    NOP = 0x00,
	    BIPUSH,
	    IPUSH,
	    POP = 0x04,
 	    POP2, 
	    POPN, 
	    DUP,  
	    DUP2,
	    LOADC, 
	    LOADA,  
	    NEW, 
        SNEW, 
        ILOAD = 0x10,
        DLOAD,
        ALOAD, 
        IALOAD = 0x18,   
        DALOAD,
        AALOAD,
        ISTORE = 0x20,
        DSTORE,
        ASTORE,
        IASTORE = 0x28,
        DASTORE,
        AASTORE,
        IADD = 0x30,
        DADD,
        ISUB = 0x34,
        DSUB,
        IMUL = 0x38,
        DMUL,
        IDIV = 0x3c,
        DDIV,
        INEG = 0x40,
        DNEG,
        ICMP = 0x44,
        DCMP,
        I2D = 0x60, 
        D2I,
        I2C,
        JMP = 0x70,
        JE,
        JNE,
        JL,
        JGE,
        JG,
        JLE,
        CALL = 0x80,
        RET = 0x88,
        IRET,
        DRET,
        ARET,
        IPRINT=0xa0,
        DPRINT,
        CPRINT,
        SPRINT,
        PRINTL=0xaf,
        ISCAN=0xb0,
        DSCAN,
        CSCAN,
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