#pragma once

#include <cstdint>
#include <thread>
#include <atomic>
#include <future>
#include <optional>
#include <variant>
#include <queue>
#include <mutex>
#include <istream>
#include <ostream>
#include <filesystem>

namespace x69::emu::peripheral::terminal
{
	void open_terminal(uint8_t _width, uint8_t _height);
	void close_terminal();

	void set_character(char _c, uint8_t _x, uint8_t _y);
	char get_character(uint8_t _x, uint8_t _y);


	void clear_terminal();


	enum class SPECIAL_REG
	{
		PC,
		LR,
		SP,
		ADDR
	};

	void update_register_value(uint8_t _reg, uint8_t _val);
	void update_special_register(SPECIAL_REG _reg, uint16_t _val);

}

namespace x69::emu
{

	class Terminal
	{
	protected:
		void set_open_flag(bool _b) noexcept;
		constexpr static inline std::chrono::milliseconds TIMEOUT_DEFAULT{ 100 };

	public:
		class Message
		{
		public:
			enum MESSAGE_TYPE_E
			{
				GOT_INPUT,
				EXITED,
				STEP
			};

		private:
			using MESSAGE_TYPE = MESSAGE_TYPE_E;

		public:
			template <MESSAGE_TYPE>
			struct MessageType;

			template <>
			struct MessageType<MESSAGE_TYPE::GOT_INPUT>
			{
				std::string str_;
			};
			using GotInput = MessageType<MESSAGE_TYPE::GOT_INPUT>;

			template <>
			struct MessageType<MESSAGE_TYPE::EXITED> {};
			using Exited = MessageType<MESSAGE_TYPE::EXITED>;

			template <>
			struct MessageType<MESSAGE_TYPE::STEP> {};
			using Step = MessageType<MESSAGE_TYPE::STEP>;


		private:
			using variant_type = std::variant<
				MessageType<MESSAGE_TYPE::GOT_INPUT>,
				MessageType<MESSAGE_TYPE::EXITED>,
				MessageType<MESSAGE_TYPE::STEP>
			>;

		public:

			MESSAGE_TYPE index() const noexcept
			{
				return (MESSAGE_TYPE)this->vt_.index();
			};

			template <MESSAGE_TYPE Type>
			auto& get()
			{
				return std::get<MessageType<Type>>(this->vt_);
			};
			template <MESSAGE_TYPE Type>
			const auto& get() const
			{
				return std::get<MessageType<Type>>(this->vt_);
			};

			template <MESSAGE_TYPE Type>
			Message(const MessageType<Type>& _msg) : 
				vt_{ _msg }
			{};
			template <MESSAGE_TYPE Type>
			Message& operator=(const MessageType<Type>& _msg)
			{
				this->vt_ = _msg;
				return *this;
			};

			template <MESSAGE_TYPE Type>
			Message(MessageType<Type>&& _msg) :
				vt_{ std::move(_msg) }
			{};
			template <MESSAGE_TYPE Type>
			Message& operator=(MessageType<Type>&& _msg)
			{
				this->vt_ = std::move(_msg);
				return *this;
			};

		private:
			variant_type vt_;

		};

		void edit_file(const std::filesystem::path& _path);

		void push_message(const Message& _msg);
		void push_message(Message&& _msg);

		bool has_message() const noexcept;
		Message next_message();
		void ignore_message() noexcept;
		void clear_messages() noexcept;


		int thread_main();

		bool is_open() const noexcept;
		explicit operator bool() const noexcept;

		bool open(std::chrono::milliseconds _timeoutAfter);
		bool open();


		std::optional<int> close(std::chrono::milliseconds _timeoutAfter);
		std::optional<int> close();

		Terminal(std::streambuf* _einBuf, std::streambuf* _eoutBuf);
		~Terminal();

	private:

		std::istream ein_;
		std::ostream eout_;

		mutable std::mutex msg_queue_mtx_;
		std::queue<Message> msg_queue_;

		bool is_open_ = false;
		std::atomic<int> res_ = 0;
		std::thread t_;

	};

}



