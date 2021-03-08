#pragma once

#include <cstdint>

namespace x69::emu
{

	struct ALUFlags
	{
		uint8_t bits = 0x00;

		constexpr bool is_locked() const noexcept
		{
			return this->locked_;
		};

		void lock() noexcept
		{
			this->locked_ = true;
		};
		void unlock() noexcept
		{
			this->locked_ = false;
		};

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
			return (this->bits & (uint8_t)_f) != 0;
		};

		void set(FLAGS _f) noexcept
		{
			if (!this->is_locked())
			{
				this->bits |= (uint8_t)_f;
			};
		};
		void clear(FLAGS _f) noexcept
		{
			this->bits &= ~(uint8_t)_f;
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
			if (!this->is_locked())
			{
				this->bits = (_to) ? 0xFF : 0x00;
			};
		};

		void clear_all() noexcept
		{
			this->set_all_to(false);
		};
		void set_all() noexcept
		{
			this->set_all_to(true);
		};

	private:
		bool locked_ = false;

	};


};
