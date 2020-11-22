#include <x69EmulatorLib.h>

#include <iostream>
#include <array>
#include <cstdint>
#include <optional>
#include <cassert>
#include <numeric>
#include <vector>
#include <fstream>
#include <filesystem>
#include <string>

template <typename T>
constexpr static inline T twos(T _v) noexcept
{
	return !_v + 1;
};

#if defined(WIN32) || defined(_WIN32) || defined(_WIN32_) || defined(_NT_)

#include <Windows.h>

#undef min
#undef max

#pragma region ANSI_CONSTANTS

namespace ccodes
{
	namespace impl
	{
		HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	};

	constexpr static inline auto NONE = 15;
	constexpr static inline auto BLACK = 0;
	constexpr static inline auto RED = 12;
	constexpr static inline auto GREEN = 10;
	constexpr static inline auto YELLOW = 14;
	constexpr static inline auto BLUE = 9;
	//constexpr static inline auto MAGENTA = "\033[35m";
	//constexpr static inline auto CYAN = "\033[36m";
	constexpr static inline auto WHITE = 15;
	//constexpr static inline auto BOLDBLACK = "\033[1m\033[30m";
	//constexpr static inline auto BOLDRED = "\033[1m\033[31m";
	//constexpr static inline auto BOLDGREEN = "\033[1m\033[32m";
	//constexpr static inline auto BOLDYELLOW = "\033[1m\033[33m";
	//constexpr static inline auto BOLDBLUE = "\033[1m\033[34m";
	//constexpr static inline auto BOLDMAGENTA = "\033[1m\033[35m";
	//constexpr static inline auto BOLDCYAN = "\033[1m\033[36m";
	//constexpr static inline auto BOLDWHITE = "\033[1m\033[37m";

	void set_text_color(std::ostream& _ostr, int _col)
	{
		SetConsoleTextAttribute(impl::hConsole, _col);
	};

};

#pragma endregion ANSI_CONSTANTS


#elif defined(__linux__) || defined(__unix__)

#pragma region ANSI_CONSTANTS

namespace ccodes
{

	constexpr static inline auto NONE = "\033[0m";
	constexpr static inline auto BLACK = "\033[30m";      /* Black */
	constexpr static inline auto RED = "\033[31m";      /* Red */
	constexpr static inline auto GREEN = "\033[32m";      /* Green */
	constexpr static inline auto YELLOW = "\033[33m";      /* Yellow */
	constexpr static inline auto BLUE = "\033[34m";      /* Blue */
	//constexpr static inline auto MAGENTA = "\033[35m";      /* Magenta */
	//constexpr static inline auto CYAN = "\033[36m";      /* Cyan */
	constexpr static inline auto WHITE = "\033[37m";      /* White */
	//constexpr static inline auto BOLDBLACK = "\033[1m\033[30m";      /* Bold Black */
	//constexpr static inline auto BOLDRED = "\033[1m\033[31m";      /* Bold Red */
	//constexpr static inline auto BOLDGREEN = "\033[1m\033[32m";      /* Bold Green */
	//constexpr static inline auto BOLDYELLOW = "\033[1m\033[33m";      /* Bold Yellow */
	//constexpr static inline auto BOLDBLUE = "\033[1m\033[34m";      /* Bold Blue */
	//constexpr static inline auto BOLDMAGENTA = "\033[1m\033[35m";      /* Bold Magenta */
	//constexpr static inline auto BOLDCYAN = "\033[1m\033[36m";      /* Bold Cyan */
	//constexpr static inline auto BOLDWHITE = "\033[1m\033[37m";      /* Bold White */

	void set_text_color(std::ostream& _ostr, const char* _col)
	{
		_ostr << _col;
	};

};

#pragma endregion ANSI_CONSTANTS

#else

namespace ccodes
{
	void set_text_color(std::ostream& _ostr, const char* _col)
	{};
}


#endif

struct color
{
public:
	enum COLORS
	{
		RED,
		BLUE,
		WHITE,
		BLACK,
		YELLOW,
		GREEN,
		NONE
	};

private:
	template <COLORS Col>
	struct color_t
	{
		friend inline std::ostream& operator<<(std::ostream& _ostr, const color_t<Col>& _col)
		{
			if constexpr (Col == COLORS::NONE)
			{
				ccodes::set_text_color(_ostr, ccodes::NONE);
			}
			else if constexpr (Col == COLORS::RED)
			{
				ccodes::set_text_color(_ostr, ccodes::RED);
			}
			else if constexpr (Col == COLORS::BLUE)
			{
				ccodes::set_text_color(_ostr, ccodes::BLUE);
			}
			else if constexpr (Col == COLORS::GREEN)
			{
				ccodes::set_text_color(_ostr, ccodes::GREEN);
			}
			else if constexpr (Col == COLORS::WHITE)
			{
				ccodes::set_text_color(_ostr, ccodes::WHITE);
			}
			else if constexpr (Col == COLORS::BLACK)
			{
				ccodes::set_text_color(_ostr, ccodes::BLACK);
			}
			else if constexpr (Col == COLORS::YELLOW)
			{
				ccodes::set_text_color(_ostr, ccodes::YELLOW);
			}
			else
			{
				static_assert(false);
			};

			return _ostr;
		};
	};

public:

	static inline constexpr auto none = color_t<COLORS::NONE>{};
	static inline constexpr auto red = color_t<COLORS::RED>{};
	static inline constexpr auto blue = color_t<COLORS::BLUE>{};
	static inline constexpr auto white = color_t<COLORS::WHITE>{};
	static inline constexpr auto black = color_t<COLORS::BLACK>{};
	static inline constexpr auto yellow = color_t<COLORS::YELLOW>{};
	static inline constexpr auto green = color_t<COLORS::GREEN>{};

};





template <typename T>
struct Register
{
	using value_type = T;

	operator T& () noexcept { return this->val_; };
	constexpr operator const T& () const noexcept { return this->val_; };

	Register& operator=(T _val) noexcept
	{
		this->val_ = _val;
		return *this;
	};

	T val_;
};

template <uint8_t GeneralRegisters, typename WordType, typename DirectMemAddrType>
struct CPUTraits
{
	using word_type = WordType;
	using register_type = Register<typename word_type>;

	using direct_address_type = DirectMemAddrType;

	constexpr static inline uint8_t word_size() { return sizeof(WordType); };
	constexpr static inline uint8_t general_registers() { return GeneralRegisters; };
	constexpr static inline size_t direct_memory_size() { return std::numeric_limits<DirectMemAddrType>::max(); };

};

using CPUVersionTraits = CPUTraits<16, uint8_t, uint16_t>;

struct Instruction
{
public:
	using cpu_traits = CPUVersionTraits;
	using word_type = typename cpu_traits::word_type;

	word_type ibyte; // first instruction byte, I call it the instruction byte
	word_type dbyte; // second instruction byte, I call it the data byte
	std::optional<word_type> ebyte; // optional third instruction byte, I call it the extra byte

	constexpr Instruction() noexcept : 
		ibyte{ 0x00 }, dbyte{ 0x00 }, ebyte{ std::nullopt }
	{};
	constexpr Instruction(word_type _ibyte, word_type _dbyte, word_type _ebyte) noexcept :
		ibyte{ _ibyte }, dbyte{ _dbyte }, ebyte{ _ebyte }
	{}; 
	constexpr Instruction(word_type _ibyte, word_type _dbyte) noexcept :
		ibyte{ _ibyte }, dbyte{ _dbyte }, ebyte{ std::nullopt }
	{};

};

struct CPURegisters
{
public:
	using cpu_traits = CPUVersionTraits;
	using word_type = typename cpu_traits::word_type;
	using register_type = typename cpu_traits::register_type;

	using value_type = register_type;

private:
	using ContainerT = std::array<value_type, cpu_traits::general_registers()>;

public:
	using iterator = typename ContainerT::iterator;
	using const_iterator = typename ContainerT::const_iterator;
	
	using reverse_iterator = typename ContainerT::reverse_iterator;
	using const_reverse_iterator = typename ContainerT::const_reverse_iterator;

	constexpr iterator begin() noexcept { return this->regs_.begin(); };
	constexpr const_iterator begin() const noexcept { return this->regs_.cbegin(); };
	constexpr const_iterator cbegin() const noexcept { return this->regs_.cbegin(); };

	constexpr iterator end() noexcept { return this->regs_.end(); };
	constexpr const_iterator end() const noexcept { return this->regs_.cend(); };
	constexpr const_iterator cend() const noexcept { return this->regs_.cend(); };

	constexpr reverse_iterator rbegin() noexcept { return this->regs_.rbegin(); };
	constexpr const_reverse_iterator rbegin() const noexcept { return this->regs_.crbegin(); };
	constexpr const_reverse_iterator crbegin() const noexcept { return this->regs_.crbegin(); };

	constexpr reverse_iterator rend() noexcept { return this->regs_.rend(); };
	constexpr const_reverse_iterator rend() const noexcept { return this->regs_.crend(); };
	constexpr const_reverse_iterator crend() const noexcept { return this->regs_.crend(); };

	value_type& at(uint8_t _r) noexcept
	{
		return this->regs_.at(_r);
	};
	constexpr const value_type& at(uint8_t _r) const noexcept
	{
		return this->regs_.at(_r);
	};

	constexpr value_type read(uint8_t _r) const noexcept
	{
		return this->at(_r);
	};
	void write(uint8_t _r, value_type _val) noexcept
	{
		this->at(_r) = _val;
	};

	value_type& operator[](uint8_t _r) noexcept
	{
		return this->at(_r);
	};
	constexpr const value_type& operator[](uint8_t _r) const noexcept
	{
		return this->at(_r);
	};

	friend inline std::ostream& operator<<(std::ostream& _ostr, CPURegisters& _regs)
	{
		uint8_t _rnum = 0;
		for (auto& r : _regs)
		{
			_ostr << color::blue << "r" << (int)_rnum++ << color::none << " : 0x" << std::hex << (int)r << std::dec << "\n";
		};
		return _ostr;
	};

	constexpr size_t size() const noexcept { return this->regs_.size(); };

private:
	ContainerT regs_{};

};

struct ALUFlags
{
	uint8_t bits = 0x00;

	enum FLAGS : uint8_t 
	{
		ZERO = 0x01,
		Z = ZERO,
		CARRY = 0x02,
		C = CARRY,
		TWOS = 0x04,
		T = TWOS
	};

	constexpr bool get(FLAGS _f) const noexcept
	{
		return this->bits & (uint8_t)_f;
	};

	void set(FLAGS _f) noexcept
	{
		this->bits |= (uint8_t)_f;
	};
	void clear(FLAGS _f) noexcept
	{
		this->bits &= !(uint8_t)_f;
	};

	void set_to(FLAGS _f, bool _to) noexcept
	{
		if (_to)
		{
			this->set(_f);
		}
		else
		{
			this->clear(_f);
		};
	};
	
	void set_all_to(bool _to) noexcept
	{
		this->bits = (_to) ? 0xFF : 0x00;
	};

	void clear_all() noexcept
	{
		this->set_all_to(false);
	};
	void set_all() noexcept
	{
		this->set_all_to(true);
	};
	
};

struct SpecialRegisters
{
public:
	using cpu_traits = CPUVersionTraits;
	using word_type = typename cpu_traits::word_type;
	using register_type = typename cpu_traits::register_type;
	
	using value_type = register_type;

	enum REGISTERS : uint8_t
	{
		PC = 0x0,
		LR = 0x1,
		SP = 0x2,
		ADDR = 0x3
	};

private:
	using ContainerT = std::array<value_type, 4>;
	ContainerT regs_{};

public:

	using iterator = typename ContainerT::iterator;
	using const_iterator = typename ContainerT::const_iterator;

	using reverse_iterator = typename ContainerT::reverse_iterator;
	using const_reverse_iterator = typename ContainerT::const_reverse_iterator;

	constexpr iterator begin() noexcept { return this->regs_.begin(); };
	constexpr const_iterator begin() const noexcept { return this->regs_.cbegin(); };
	constexpr const_iterator cbegin() const noexcept { return this->regs_.cbegin(); };

	constexpr iterator end() noexcept { return this->regs_.end(); };
	constexpr const_iterator end() const noexcept { return this->regs_.cend(); };
	constexpr const_iterator cend() const noexcept { return this->regs_.cend(); };

	constexpr reverse_iterator rbegin() noexcept { return this->regs_.rbegin(); };
	constexpr const_reverse_iterator rbegin() const noexcept { return this->regs_.crbegin(); };
	constexpr const_reverse_iterator crbegin() const noexcept { return this->regs_.crbegin(); };

	constexpr reverse_iterator rend() noexcept { return this->regs_.rend(); };
	constexpr const_reverse_iterator rend() const noexcept { return this->regs_.crend(); };
	constexpr const_reverse_iterator crend() const noexcept { return this->regs_.crend(); };

	value_type& at(REGISTERS _r) noexcept
	{
		return this->regs_.at(_r);
	};
	constexpr const value_type& at(REGISTERS _r) const noexcept
	{
		return this->regs_.at(_r);
	};

	constexpr value_type read(REGISTERS _r) const noexcept
	{
		return this->at(_r);
	};
	void write(REGISTERS _r, word_type _val) noexcept
	{
		this->at(_r) = _val;
	};

	value_type& operator[](REGISTERS _r) noexcept
	{
		return this->at(_r);
	};
	constexpr const value_type& operator[](REGISTERS _r) const noexcept
	{
		return this->at(_r);
	};

	friend inline std::ostream& operator<<(std::ostream& _ostr, SpecialRegisters& _regs)
	{
		_ostr << color::yellow << "pc  " << color::none << " : 0x" << std::hex << (int)_regs.at(PC) << std::dec << "\n";
		_ostr << color::yellow << "lr  " << color::none <<	" : 0x" << std::hex << (int)_regs.at(LR) << std::dec << "\n";
		_ostr << color::yellow << "sp  " << color::none << " : 0x" << std::hex << (int)_regs.at(SP) << std::dec << "\n";
		_ostr << color::yellow << "addr" << color::none << " : 0x" << std::hex << (int)_regs.at(ADDR) << std::dec << "\n";
		return _ostr;
	};

	constexpr size_t size() const noexcept { return this->regs_.size(); };

};

struct Memory
{
public:
	using cpu_traits = CPUVersionTraits;
	using word_type = typename cpu_traits::word_type;
	using address_type = cpu_traits::direct_address_type;
	
private:
	using ContainerT = std::array<word_type, cpu_traits::direct_memory_size()>;

public:
	using iterator = typename ContainerT::iterator;
	using const_iterator = typename ContainerT::const_iterator;

	using reverse_iterator = typename ContainerT::reverse_iterator;
	using const_reverse_iterator = typename ContainerT::const_reverse_iterator;

	constexpr iterator begin() noexcept { return this->data_.begin(); };
	constexpr const_iterator begin() const noexcept { return this->data_.cbegin(); };
	constexpr const_iterator cbegin() const noexcept { return this->data_.cbegin(); };

	constexpr iterator end() noexcept { return this->data_.end(); };
	constexpr const_iterator end() const noexcept { return this->data_.cend(); };
	constexpr const_iterator cend() const noexcept { return this->data_.cend(); };

	constexpr reverse_iterator rbegin() noexcept { return this->data_.rbegin(); };
	constexpr const_reverse_iterator rbegin() const noexcept { return this->data_.crbegin(); };
	constexpr const_reverse_iterator crbegin() const noexcept { return this->data_.crbegin(); };

	constexpr reverse_iterator rend() noexcept { return this->data_.rend(); };
	constexpr const_reverse_iterator rend() const noexcept { return this->data_.crend(); };
	constexpr const_reverse_iterator crend() const noexcept { return this->data_.crend(); };

	constexpr word_type read(address_type _addr) const noexcept
	{
		return this->data_.at(_addr);
	};
	void write(address_type _addr, word_type _data) noexcept
	{
		this->data_.at(_addr) = _data;
	};

private:
	ContainerT data_{};
};



struct CPU
{
public:
	using cpu_traits = CPUVersionTraits;
	using word_type = typename cpu_traits::word_type;
	
private:
	SpecialRegisters sregs_{};
	CPURegisters registers_{};
	ALUFlags alu_flags_{};
	Memory* direct_mem_ = nullptr;

	bool pc_lock_ = false;

public:
	Memory* direct_memory() const noexcept { return this->direct_mem_; };

protected:
	struct bit_masks
	{
		constexpr static inline word_type CPU_OP =		0b01000000;
		constexpr static inline word_type ALUREG_OP =	0b00100000;
		constexpr static inline word_type MEMORY_OP =	0b00010000;

		constexpr static inline word_type USE_EXTRA_BYTE =	0b10000000;

		constexpr static inline word_type CPU_CONDITIONAL_OP =			0b00100000;
		constexpr static inline word_type CPU_RESERVED_OP =				0b00010000;
		constexpr static inline word_type CPU_SPECIAL_REGISTERS_OP =	0b00001000;

	};

	enum class CPU_BASIC_OPS : word_type
	{
		JMP = 0b00,
		CALL = 0b01,
		RJMP = 0b10,
		RCALL = 0b11
	};

private:

	void preform_jump_op(Instruction _ins)
	{
		// Basic CPU op

			// Build memory address
			// If no extra byte is provided, load in the two registers provided by the dbyte
			// If an extra byte is provided, build the address by concating the dbyte(lower byte) and ebyte(upper byte)
		Memory::address_type _address = (_ins.ebyte) ?
			((Memory::address_type)_ins.dbyte | ((Memory::address_type)*_ins.ebyte << 8)) :
			((Memory::address_type)this->registers()[_ins.dbyte & 0x0F] | ((Memory::address_type)this->registers()[(_ins.dbyte & 0xF0) >> 4] << 8));

		// Determine instruction and preform it
		switch (_ins.ibyte & 0b00000011)
		{
		case (word_type)CPU_BASIC_OPS::JMP:
			this->pc_lock_ = true;
			this->special_regs()[SpecialRegisters::PC] = _address;
			break;
		case (word_type)CPU_BASIC_OPS::RJMP:
			this->pc_lock_ = true;
			this->special_regs()[SpecialRegisters::PC] += _address;
			break;
		case (word_type)CPU_BASIC_OPS::CALL:
			this->pc_lock_ = true;
			this->special_regs()[SpecialRegisters::LR] = this->special_regs()[SpecialRegisters::PC];
			this->special_regs()[SpecialRegisters::PC] = _address;
			break;
		case (word_type)CPU_BASIC_OPS::RCALL:
			this->pc_lock_ = true;
			this->special_regs()[SpecialRegisters::LR] = this->special_regs()[SpecialRegisters::PC];
			this->special_regs()[SpecialRegisters::PC] += _address;
			break;
		default:
			abort();
			break;
		};

	};

protected:

	void handle_cpu_op(Instruction _ins)
	{

		if ((_ins.ibyte & bit_masks::CPU_CONDITIONAL_OP) != 0)
		{
			// Conditional OP

			// Get alu flag code
			auto _flagCode = (_ins.ibyte & 0b00001100) >> 2;
			ALUFlags::FLAGS _flag = (ALUFlags::FLAGS)(0x1 << _flagCode);

			// Get not or regular bit
			bool _invertState = (_ins.ibyte & 0xb00010000) != 0;

			// Check condition 
			bool _conditionMet = (_invertState ^ this->alu_flags_.get(_flag));

			// Preform if met
			if (_conditionMet)
			{
				this->preform_jump_op(_ins);
			};
		}
		else if ((_ins.ibyte & bit_masks::CPU_RESERVED_OP) != 0)
		{
			// Reserved

		}
		else if ((_ins.ibyte & bit_masks::CPU_SPECIAL_REGISTERS_OP) != 0)
		{
			// Special registers

			// false = read
			// true = write
			bool _readOrWrite = _ins.ibyte & 0b00000100;
			uint8_t _sregCode = _ins.ibyte & 0b00000011;

			auto& _sreg = this->special_regs()[(SpecialRegisters::REGISTERS)_sregCode];
			
			if (_readOrWrite)
			{
				// Write
				Memory::address_type _val = (_ins.ebyte) ?
					((Memory::address_type)_ins.dbyte | ((Memory::address_type)*_ins.ebyte << 8)) :
					((Memory::address_type)this->registers()[_ins.dbyte & 0x0F] | ((Memory::address_type)this->registers()[(_ins.dbyte & 0xF0) >> 4] << 8));
				_sreg = _val;
			}
			else
			{
				// Read

				auto& _ra = this->registers()[_ins.dbyte & 0x0F];
				auto& _rb = this->registers()[(_ins.dbyte & 0xF0) >> 4];

				_ra = (word_type)(_sreg & 0x00FF);
				_rb = (word_type)((_sreg & 0xFF00) >> 16);

			};

		}
		else
		{
			// Basic CPU op
	
			this->preform_jump_op(_ins);

		};

	};

	enum class ALU_OPS : uint8_t
	{
		CLR = 0x00,
		SER = 0x10,
		NOT = 0x01,
		TWO = 0x11,
		AND = 0x02,
		NAND = 0x12,
		ORR = 0x03,
		NOR = 0x13,
		XOR = 0x04,
		XNOR = 0x14,
		ADD = 0x05,
		ADC = 0x15,
		SUB = 0x06,
		SBC = 0x16,
		INC = 0x07,
		DEC = 0x17,
		MOV = 0x08,
		MVN = 0x18,
		SET = 0x09,
		STN = 0x19,
		CMP = 0x0A,
		MAX_USED_VALUE = CMP
	};

	void handle_alureg_op(Instruction _ins)
	{
		uint8_t _opBits = _ins.ibyte & 0x1F;
		assert(_opBits <= (uint8_t)ALU_OPS::MAX_USED_VALUE);

		ALU_OPS _op = (ALU_OPS)_opBits;

		auto& _ra = registers_[_ins.dbyte & 0x0F];
		auto& _rb = registers_[(_ins.dbyte & 0xF0) >> 4];

		word_type _a = _ra;
		word_type _b = (_ins.ebyte) ? *_ins.ebyte : _rb;

		ALUFlags _flags{ this->alu_flags_ };
		this->alu_flags_.clear_all();

		switch (_op)
		{
		case ALU_OPS::CLR:
			_rb = 0x00;
			this->alu_flags_.set_to(ALUFlags::Z, _rb == 0);
			break;
		case ALU_OPS::SER:
			_rb = 0xFF;
			this->alu_flags_.set_to(ALUFlags::Z, _rb == 0);
			break;
		case ALU_OPS::NOT:
			_rb = !_a;
			this->alu_flags_.set_to(ALUFlags::Z, _rb == 0);
			break;
		case ALU_OPS::TWO:
			_rb = (!_a) + 1;
			this->alu_flags_.set_to(ALUFlags::Z, _rb == 0);
			break;
		case ALU_OPS::AND:
			_rb = _a & _b;
			this->alu_flags_.set_to(ALUFlags::Z, _rb == 0);
			break;
		case ALU_OPS::NAND:
			_rb = !(_a & _b);
			this->alu_flags_.set_to(ALUFlags::Z, _rb == 0);
			break;
		case ALU_OPS::ORR:
			_rb = _a | _b;
			this->alu_flags_.set_to(ALUFlags::Z, _rb == 0);
			break;
		case ALU_OPS::NOR:
			_rb = !(_a | _b);
			this->alu_flags_.set_to(ALUFlags::Z, _rb == 0);
			break;
		case ALU_OPS::XOR:
			_rb = _a ^ _b;
			this->alu_flags_.set_to(ALUFlags::Z, _rb == 0);
			break;
		case ALU_OPS::XNOR:
			_rb = !(_a ^ _b);
			this->alu_flags_.set_to(ALUFlags::Z, _rb == 0);
			break;
		case ALU_OPS::ADD:
			this->alu_flags_.set_to(ALUFlags::C, (_a + _b) < _b);
			_rb = _a + _b;
			this->alu_flags_.set_to(ALUFlags::Z, _rb == 0);
			break;
		case ALU_OPS::ADC:
			if (_flags.get(ALUFlags::C))
			{
				this->alu_flags_.set_to(ALUFlags::C, (_a + _b + 1) < _b);
				_rb = _a + _b + 1;
			}
			else
			{
				this->alu_flags_.set_to(ALUFlags::C, (_a + _b) < _b);
				_rb = _a + _b;
			};
			this->alu_flags_.set_to(ALUFlags::Z, _rb == 0);
			break;
		case ALU_OPS::SUB:
			this->alu_flags_.set_to(ALUFlags::C, (_a + twos(_b)) < twos(_b));
			this->alu_flags_.set_to(ALUFlags::T, _a >= _b);
			_rb = _a + twos(_b);
			this->alu_flags_.set_to(ALUFlags::Z, _rb == 0);
			break;
		case ALU_OPS::SBC:
			if (_flags.get(ALUFlags::C))
			{
				this->alu_flags_.set_to(ALUFlags::C, (_a + twos(_b)) < twos(_b));
				this->alu_flags_.set_to(ALUFlags::T, _a >= _b);
				_rb = _a + twos(_b + 1);
			}
			else
			{
				this->alu_flags_.set_to(ALUFlags::C, (_a + twos(_b)) < twos(_b));
				this->alu_flags_.set_to(ALUFlags::T, _a >= _b);
				_rb = _a + twos(_b);
			};
			this->alu_flags_.set_to(ALUFlags::Z, _rb == 0);
			break;
		case ALU_OPS::INC:
			this->alu_flags_.set_to(ALUFlags::C, _b == std::numeric_limits<word_type>::max());
			_rb = _b + 1;
			this->alu_flags_.set_to(ALUFlags::Z, _rb == 0);
			break;
		case ALU_OPS::DEC:
			this->alu_flags_.set_to(ALUFlags::C, _b == std::numeric_limits<word_type>::min());
			_rb = _b + twos((word_type)0x1);
			this->alu_flags_.set_to(ALUFlags::Z, _rb == 0);
			break;
		case ALU_OPS::MOV:
			_rb = _a;
			this->alu_flags_.set_to(ALUFlags::Z, _rb == 0);
			break;
		case ALU_OPS::MVN:
			_rb = !_a;
			this->alu_flags_.set_to(ALUFlags::Z, _rb == 0);
			break;
		case ALU_OPS::SET:
			_rb = _b;
			this->alu_flags_.set_to(ALUFlags::Z, _rb == 0);
			break;
		case ALU_OPS::STN:
			_rb = !_b;
			this->alu_flags_.set_to(ALUFlags::Z, _rb == 0);
			break;
		case ALU_OPS::CMP:
			this->alu_flags_.set_to(ALUFlags::C, (_a + twos(_b)) < twos(_b));
			this->alu_flags_.set_to(ALUFlags::T, _a >= _b);
			this->alu_flags_.set_to(ALUFlags::Z, (_a + twos(_b)) == 0);
			break;
		default:
			abort();
		};

	};

	void handle_memory_op(Instruction _ins)
	{

	};
	
	void handle_privs_op(Instruction _ins)
	{

	};

	Instruction fetch_next_instruction()
	{
		Instruction _ins{};
		_ins.ibyte = this->direct_memory()->read(this->special_regs()[SpecialRegisters::PC]);
		_ins.dbyte = this->direct_memory()->read(this->special_regs()[SpecialRegisters::PC] + 1);
		
		if ((_ins.ibyte & bit_masks::USE_EXTRA_BYTE) != 0)
		{
			_ins.ebyte = this->direct_memory()->read(this->special_regs()[SpecialRegisters::PC] + 2);
		};

		return _ins;
	};

public:

	CPURegisters& registers() noexcept { return this->registers_; };
	const CPURegisters& registers() const noexcept { return this->registers_; };

	SpecialRegisters& special_regs() noexcept { return this->sregs_; };
	const SpecialRegisters& special_regs() const noexcept { return this->sregs_; };

	void process_instruction(Instruction _ins)
	{
		bool _useExtra = (_ins.ibyte & bit_masks::USE_EXTRA_BYTE) != 0;
		if (_useExtra)
		{
			assert(_ins.ebyte);
		};

		if ((_ins.ibyte & bit_masks::CPU_OP) != 0)
		{
			// CPU Module Op Code
			handle_cpu_op(_ins);
		}
		else if ((_ins.ibyte & bit_masks::ALUREG_OP) != 0)
		{
			// ALU/Regs Module Op Code
			handle_alureg_op(_ins);
		}
		else if ((_ins.ibyte & bit_masks::MEMORY_OP) != 0)
		{
			// Memory Module Op Code
			handle_memory_op(_ins);
		}
		else
		{
			// Privs Module Op Code
			handle_privs_op(_ins);
		};

	};

	void step()
	{
		auto _ins = this->fetch_next_instruction();
		this->process_instruction(_ins);

		Memory::address_type _pcInc = 2;
		if (_ins.ebyte)
		{
			++_pcInc;
		};

		if (!this->pc_lock_)
		{
			this->special_regs()[SpecialRegisters::PC] += _pcInc;
		}
		else
		{
			this->pc_lock_ = false;
		};

	};

	CPU(Memory* _directMem) :
		direct_mem_{ _directMem }
	{};

};

Memory direct_memory{};

std::vector<uint8_t> load_x69_machine_code(const std::filesystem::path& _path)
{
	std::cout << "Reading " << _path << '\n';
	assert(std::filesystem::exists(_path));
	if (_path.is_relative())
		std::filesystem::relative(_path);

	std::ifstream _ifstr{ _path, std::ifstream::binary };
	assert(_ifstr.is_open());

	std::vector<uint8_t> _bytes{};
	char _buff[256]{};
	while (!_ifstr.eof())
	{
		_ifstr.read(_buff, sizeof(_buff) / sizeof(char));
		_bytes.insert(_bytes.end(), _buff, _buff + _ifstr.gcount());
	};

	_ifstr.close();

	return _bytes;
};

int imain(int argc, char* argv[], char* envp[])
{
	std::cout << "x69 Emulator v0.0.1\n";

	std::vector<std::string> _args{};
	for (int i = 1; i < argc; ++i)
	{
		std::cout << argv[i] << '\n';
		_args.push_back(argv[i]);
	};

	std::filesystem::path _bootFile{ "./boot.o" };

	std::cout << "arg count = " << _args.size() << '\n';

	for (auto it = _args.begin(); it != _args.end(); ++it)
	{
		const auto& arg = *it;
		if (arg == "-help")
		{
			std::cout << "Usage:\n [-boot <path to boot code>](default to boot.o)\n";
		}
		else if (arg.front() == '-')
		{
			const auto& _opt = arg;
			auto _param = it;

			if (it < _args.end() - 1)
			{
				_param = it + 1;
				if (_opt == "-boot")
				{
					_bootFile = std::filesystem::relative(*_param);
					std::cout << "Set build file to : \n";
				}
				else
				{
					std::cout << "Unrecognized arguement : " << _opt << '\n';
					abort();
				};
			};

		};
	};

	CPU _cpu{ &direct_memory };

	auto _codeData = load_x69_machine_code(_bootFile);
	std::copy(_codeData.begin(), _codeData.end(), direct_memory.begin());

	for (int i = 0; i < 200; ++i)
	{
		_cpu.step();
	};

	std::cout << _cpu.registers() << '\n';
	std::cout << _cpu.special_regs() << '\n';

	return 0;
}

int main(int argc, char* argv[], char* envp[])
{
	return imain(argc, argv, envp);
};
