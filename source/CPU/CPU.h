#pragma once

#include "Spec/Spec.h"
#include "Register/Register.h"
#include "Memory/Memory.h"
#include "ALU/ALU.h"
#include "Instruction/Instruction.h"

#include <cassert>
#include <array>

namespace x69::emu
{
	struct SpecialRegisters
	{
	public:
		using cpu_traits = CPUVersionTraits;
		using word_type = typename cpu_traits::word_type;
		using register_type = Register<typename cpu_traits::direct_address_type>;

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

		constexpr size_t size() const noexcept { return this->regs_.size(); };

	};
};

namespace x69::emu
{
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

		constexpr size_t size() const noexcept { return this->regs_.size(); };

	private:
		ContainerT regs_{};

	};
};


namespace x69::emu
{
	struct PrivilegeFlag
	{
	public:
		bool check() const noexcept { return this->os_mode_flag_; };
		explicit operator bool() const noexcept { return this->check(); };

		void set() noexcept { this->os_mode_flag_ = true; };
		void clear() noexcept { this->os_mode_flag_ = false; };



	private:
		bool os_mode_flag_ = true;
	};

	template <typename T>
	constexpr static inline T twos(T _v) noexcept
	{
		return !_v + 1;
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
		PrivilegeFlag privs_{};

		bool pc_lock_ = false;

	public:
		Memory* direct_memory() const noexcept { return this->direct_mem_; };

	protected:
		struct bit_masks
		{
			constexpr static inline word_type CPU_OP = 0b01000000;
			constexpr static inline word_type ALUREG_OP = 0b00100000;
			constexpr static inline word_type MEMORY_OP = 0b00010000;

			constexpr static inline word_type USE_EXTRA_BYTE = 0b10000000;

			constexpr static inline word_type CPU_CONDITIONAL_OP = 0b00100000;
			constexpr static inline word_type CPU_RESERVED_OP = 0b00010000;
			constexpr static inline word_type CPU_SPECIAL_REGISTERS_OP = 0b00001000;

		};

		enum class CPU_BASIC_OPS : word_type
		{
			JMP = 0b00,
			CALL = 0b01,
			RJMP = 0b10,
			RCALL = 0b11
		};

		enum class CPU_GENERAL_OPS : word_type
		{
			RET = 0b0000,
			DNFG = 0b0001,
			ENFG = 0b0010,
			PUSH = 0b0011,
			POP = 0b0100
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

			Memory::address_type _lrMayBe = this->special_regs()[SpecialRegisters::PC].val_;
			_lrMayBe += 2;
			if (_ins.ebyte)
			{
				++_lrMayBe;
			};

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
				this->special_regs()[SpecialRegisters::LR] = _lrMayBe;
				this->special_regs()[SpecialRegisters::PC] = _address;
				break;
			case (word_type)CPU_BASIC_OPS::RCALL:
				this->pc_lock_ = true;
				this->special_regs()[SpecialRegisters::LR] = _lrMayBe;
				this->special_regs()[SpecialRegisters::PC] += _address;
				break;
			default:
				abort();
				break;
			};

		};

		Memory::address_type form_address(const Instruction& _ins) const noexcept
		{
			return (_ins.ebyte) ?
				((Memory::address_type)_ins.dbyte | ((Memory::address_type)*_ins.ebyte << 8)) :
				((Memory::address_type)this->registers()[_ins.dbyte & 0x0F] | ((Memory::address_type)this->registers()[(_ins.dbyte & 0xF0) >> 4] << 8));
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
				bool _invertState = (_ins.ibyte & 0b00010000) != 0;

				// Check condition 
				bool _conditionMet = (_invertState ^ this->alu_flags_.get(_flag));

				// Preform if met
				if (_conditionMet)
				{
					this->preform_jump_op(_ins);
				}
				else
				{
					// Condition not met
					bool _b = false;
				};
			}
			else if ((_ins.ibyte & bit_masks::CPU_RESERVED_OP) != 0)
			{
				// General CPU OP

				// Mask to get the relevant op code bits
				word_type _ibits = _ins.ibyte & 0b00001111;

				// Grab registers provided if needed
				word_type _regNum = (_ins.dbyte & 0x0F);
				auto& _reg = this->registers()[_regNum];

				// Handle op
				switch (_ibits)
				{
				case (word_type)CPU_GENERAL_OPS::RET:
					this->pc_lock_ = true;
					this->special_regs()[SpecialRegisters::PC] = this->special_regs()[SpecialRegisters::LR];
					break;
				case (word_type)CPU_GENERAL_OPS::DNFG:
					this->alu_flags_.lock();
					break;
				case (word_type)CPU_GENERAL_OPS::ENFG:
					this->alu_flags_.unlock();
					break;

					/*
						Stack grows from the top down
					*/
				case (word_type)CPU_GENERAL_OPS::PUSH:
					this->direct_memory()->write(this->special_regs()[SpecialRegisters::SP], _reg);
					--this->special_regs()[SpecialRegisters::SP];
					break;
				case (word_type)CPU_GENERAL_OPS::POP:
					_reg = this->direct_memory()->read(this->special_regs()[SpecialRegisters::SP]);
					++this->special_regs()[SpecialRegisters::SP];
					break;



				default:
					// non op
					abort();
					break;
				};






			}
			else if ((_ins.ibyte & bit_masks::CPU_SPECIAL_REGISTERS_OP) != 0)
			{
				// Special registers

				// false = read
				// true = write
				bool _readOrWrite = _ins.ibyte & 0b00000100;
				word_type _sregCode = _ins.ibyte & 0b00000011;

				auto _regName = (SpecialRegisters::REGISTERS)_sregCode;
				auto* _sreg = &this->special_regs()[_regName];

				if (_readOrWrite)
				{
					// Write
					Memory::address_type _val = this->form_address(_ins);
					_sreg->val_ = _val;

					if (_regName == SpecialRegisters::PC)
					{
						this->pc_lock_ = true;
					};

				}
				else
				{
					// Read

					auto& _ra = this->registers()[_ins.dbyte & 0x0F];
					auto& _rb = this->registers()[(_ins.dbyte & 0xF0) >> 4];

					_ra = (word_type)(*_sreg & 0x00FF);
					_rb = (word_type)((*_sreg & 0xFF00) >> 8);

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
			CMP = 0x0A
		};

		void handle_alureg_op(Instruction _ins)
		{
			uint8_t _opBits = _ins.ibyte & 0x1F;
			//assert(_opBits <= (uint8_t)ALU_OPS::MAX_USED_VALUE);

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
				this->alu_flags_.set_to(ALUFlags::C, (word_type)(_a + _b) < _b);
				_rb = _a + _b;
				this->alu_flags_.set_to(ALUFlags::Z, _rb == 0);
				break;
			case ALU_OPS::ADC:
				if (_flags.get(ALUFlags::C))
				{
					this->alu_flags_.set_to(ALUFlags::C, (word_type)(_a + _b + 1) < _b);
					_rb = _a + _b + 1;
				}
				else
				{
					this->alu_flags_.set_to(ALUFlags::C, (word_type)(_a + _b) < _b);
					_rb = _a + _b;
				};
				this->alu_flags_.set_to(ALUFlags::Z, _rb == 0);
				break;
			case ALU_OPS::SUB:
				this->alu_flags_.set_to(ALUFlags::C, (word_type)(_a + twos(_b)) < twos(_b));
				this->alu_flags_.set_to(ALUFlags::T, _a >= _b);
				_rb = _a + twos(_b);
				this->alu_flags_.set_to(ALUFlags::Z, _rb == 0);
				break;
			case ALU_OPS::SBC:
				if (_flags.get(ALUFlags::C))
				{
					this->alu_flags_.set_to(ALUFlags::C, (word_type)(_a + twos(_b + 1)) < twos(_b + 1));
					this->alu_flags_.set_to(ALUFlags::T, _a == (_b + 1));
					_rb = _a + twos(_b + 1);
				}
				else
				{
					this->alu_flags_.set_to(ALUFlags::C, (word_type)(_a + twos(_b)) < twos(_b));
					this->alu_flags_.set_to(ALUFlags::T, _a == _b);
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
				_rb = _b - 1;
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
				this->alu_flags_.set_to(ALUFlags::C, (word_type)(_a + twos(_b)) < twos(_b));
				this->alu_flags_.set_to(ALUFlags::T, _a >= _b);
				this->alu_flags_.set_to(ALUFlags::Z, _a == _b);
				break;
			default:
				abort();
			};

		};

		void handle_memory_op(Instruction _ins)
		{

			// Check for read / write
			bool _readOrWrite = (_ins.ibyte & 0b00000001) != 0; // true = write, false = read

			// TODO : Incrementing -  post/pre & inc/dec


			// Grab register
			word_type _regCode = (_ins.dbyte & 0x0F);
			auto& _reg = this->registers()[_regCode];

			// Grab address
			auto _address = this->special_regs()[SpecialRegisters::ADDR].val_;

			// If an extra byte was provided, it is to be used as an offset
			if (_ins.ebyte)
			{
				_address += *_ins.ebyte;
			};

			// Preform operation
			if (_readOrWrite)
			{
				// Write register to memory
				if (this->privs_.check())
				{
					this->direct_memory()->write(_address, _reg);
				}
				else
				{
					this->direct_memory()->vwrite(_address, _reg);
				};
			}
			else
			{// Write register to memory
				if (this->privs_.check())
				{
					_reg.val_ = this->direct_memory()->read(_address);
				}
				else
				{
					_reg.val_ = this->direct_memory()->vread(_address);
				};

			};

		};

		enum class PRIV_OPS : uint8_t
		{
			CLEAR_FLAG = 0,
			SET_FLAG,
			SET_VMEM_BOTTOM,
			SET_VMEM_TOP,
		};

		void handle_privs_op(Instruction _ins)
		{
			if ((_ins.ibyte & 0x07) == (uint8_t)PRIV_OPS::CLEAR_FLAG)
			{
				if (this->privs_.check())
				{
					this->privs_.clear();
				};
			}
			else if ((_ins.ibyte & 0x07) == (uint8_t)PRIV_OPS::SET_FLAG)
			{
				if (this->privs_.check())
				{
					this->privs_.set();
				};
			}
			else if ((_ins.ibyte & 0x07) == (uint8_t)PRIV_OPS::SET_VMEM_BOTTOM)
			{
				auto _addr = this->form_address(_ins);
				if (this->privs_)
				{
					auto _range = this->direct_memory()->get_vmem_range();
					_range.first = _addr;
					this->direct_mem_->set_vmem_range(_range);
				};
			}
			else if ((_ins.ibyte & 0x07) == (uint8_t)PRIV_OPS::SET_VMEM_TOP)
			{
				auto _addr = this->form_address(_ins);

				if (this->privs_)
				{
					auto _range = this->direct_memory()->get_vmem_range();
					_range.second = _addr;
					this->direct_mem_->set_vmem_range(_range);
				};
			};
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

		void step();

		CPU(Memory* _directMem) :
			direct_mem_{ _directMem }
		{};

	};

}



