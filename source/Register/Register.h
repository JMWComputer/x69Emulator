#pragma once

namespace x69::emu
{
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
}