#include <x69EmulatorLib.h>
#include <x69Terminal.h>
#include <x69Assembler.h>

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
#include <thread>
#include <chrono>
#include <unordered_map>
#include <cstdlib>
#include <sstream>

namespace em = x69::emu;


em::TerminalPeriphal term_pf_{};

uint8_t __cdecl term_pf_size()
{
	return term_pf_.size();
};
uint8_t __cdecl term_pf_read(uint8_t _offset)
{
	return term_pf_.read(_offset);
};
void __cdecl term_pf_write(uint8_t _offset, uint8_t _val)
{
	term_pf_.write(_offset, _val);
};

void load_terminal_pf(em::Memory* _mem)
{
	em::x69Peripheral x69TERMINAL_PERIPHERAL{ NULL, &term_pf_size, &term_pf_read, &term_pf_write };
	_mem->register_peripheral(std::move(x69TERMINAL_PERIPHERAL), 0xF000);
};



struct EnvironmentVars
{
	std::string as_path{};
	std::string ld_path{};
	std::string comp_path{};
};

/**
 * @brief The name of the environment file
*/
const static inline std::string ENV_FILENAME = "env.txt";

EnvironmentVars load_environment_file(std::filesystem::path _path);
EnvironmentVars load_environment_file();

/**
 * @brief Invokes the assembler at the path defined in _env
 * @param _env Emulator environment variables
 * @param _sourcePath Path to source file
 * @param _outputPath Optional output file path
 * @return same as std::system()
*/
int call_assembler(const std::filesystem::path& _sourcePath, std::optional<std::filesystem::path> _outputPath);



std::vector<std::string> split_on_delimeter(const std::string& _str, char _delim);






std::vector<std::string> split_on_delimeter(const std::string& _str, char _delim)
{
	std::vector<std::string> _out{};

	size_t _lastPos = 0;
	size_t _fpos = 0;

	while (_lastPos < _str.size())
	{
		_fpos = _str.find(_delim, _lastPos);
		if (_fpos == std::string::npos)
		{
			_fpos = _str.size();
		};

		_out.push_back(_str.substr(_lastPos, _fpos - _lastPos));
		_lastPos = _fpos + 1;

	};

	return _out;
};

EnvironmentVars load_environment_file(std::filesystem::path _path)
{
	namespace fs = std::filesystem;
	assert(fs::exists(_path));
	assert(_path.filename().string() == "env.txt");
	if (_path.is_relative())
	{
		_path = fs::relative(_path);
	};

	std::ifstream _ifstr{ _path };
	assert(_ifstr.is_open());

	char _buff[256]{};
	std::string _lstr{};
	auto _buffLen = sizeof(_buff) / sizeof(std::remove_all_extents<decltype(_buff)>::type);

	EnvironmentVars _out{};
	while (!_ifstr.eof())
	{
		// overkill is for kicks
		_ifstr.getline(_buff, _buffLen);
		_lstr.assign(_buff, _ifstr.gcount());

		auto _colonPos = _lstr.find(':');
		if (_colonPos == std::string::npos)
		{
			continue;
		};

		auto _keyStr = _lstr.substr(0, _colonPos);
		if (_colonPos >= _lstr.size())
		{
			continue;
		};

		auto _valueStr = _lstr.substr(_colonPos + 1, _lstr.size() - (_colonPos + 1));

		if (_keyStr == "as")
		{
			_out.as_path = _valueStr;
		}
		else if (_keyStr == "ld")
		{
			_out.ld_path = _valueStr;
		}
		else if (_keyStr == "comp")
		{
			_out.comp_path = _valueStr;
		};

	};

	return _out;
};
EnvironmentVars load_environment_file()
{
	auto _fpath = std::filesystem::current_path();
	_fpath.append(ENV_FILENAME);
	return load_environment_file(_fpath);
};


STARTUPINFO info = { sizeof(info) };
PROCESS_INFORMATION processInfo;


int call_assembler(const std::filesystem::path& _sourcePath, std::optional<std::filesystem::path> _outputPath)
{
	namespace fs = std::filesystem;

	if (!fs::exists(_sourcePath))
	{
		return -1;
	};

	fs::path _opath{};
	if (_outputPath)
	{
		_opath = *_outputPath;
	}
	else
	{
		_opath = _sourcePath.parent_path();
		_opath.append("out.o");
	};

	assemble_x69(_sourcePath.string().c_str(), _opath.string().c_str());


	return 0;
};



struct Emulator
{
public:
	using CommandFunction = void(*)(Emulator*, const std::string& _str);

	void load_environment(const std::filesystem::path& _fromPath);
	void load_environment();

	void insert_command(const std::string& _name, CommandFunction _f)
	{
		this->commands_.insert({ _name, _f });
	};
	void call_command(const std::string& _name, const std::string& _str)
	{
		this->commands_.at(_name)(this, _str);
	};
	bool has_command(const std::string& _name) const
	{
		return this->commands_.contains(_name);
	};

	void update();
	void process_command(const std::string& _input);

	bool load_file(const std::filesystem::path& _path);

	void dump_memory();

	void start(uint16_t _atAddress);
	void start();

	void pause();
	void resume();

	void step();

	

	auto& env() noexcept { return this->env_; };
	const auto& env() const noexcept { return this->env_; };




	auto& memory() noexcept
	{
		return this->direct_memory_;
	};
	const auto& memory() const noexcept
	{
		return this->direct_memory_;
	};

	auto& ecpu() noexcept
	{
		return this->ecpu_;
	};
	const auto& ecpu() const noexcept
	{
		return this->ecpu_;
	};

	auto& terminal() noexcept
	{
		return this->terminal_;
	};
	const auto& terminal() const noexcept
	{
		return this->terminal_;
	};

protected:
	static void cmd_pause(Emulator* _ptr, const std::string& _str)
	{
		_ptr->pause();
	};
	static void cmd_resume(Emulator* _ptr, const std::string& _str)
	{
		_ptr->resume();
	};
	static void cmd_mdump(Emulator* _ptr, const std::string& _str)
	{
		_ptr->dump_memory();
	};
	static void cmd_load(Emulator* _ptr, const std::string& _str)
	{
		auto _pathBegin = _str.find_first_not_of(' ', _str.find(' '));
		auto _pathEnd = _str.find(' ', _pathBegin);
		if (_pathEnd == std::string::npos)
		{
			_pathEnd = _str.size();
		};

		std::filesystem::path _path{ _str.substr(_pathBegin, _pathEnd - _pathBegin).c_str() };
		_ptr->load_file(_path);
	};	
	static void cmd_do(Emulator* _ptr, const std::string& _str)
	{
		cmd_load(_ptr, _str);
		_ptr->start();
	};
	static void cmd_step(Emulator* _ptr, const std::string& _str)
	{
		_ptr->step();
	};

	static void cmd_as(Emulator* _ptr, const std::string& _str)
	{
		auto _ts = split_on_delimeter(_str, ' ');
		if (_ts.size() < 2)
		{
			return;
		};

		std::optional<std::filesystem::path> _outPath = std::nullopt;
		if (_str.size() > 2)
		{
			_outPath = _ts[2];
		};

		call_assembler(_ts[1], _outPath);

	};
	static void cmd_edit(Emulator* _ptr, const std::string& _str)
	{
		auto _ts = split_on_delimeter(_str, ' ');

		if (_ts.size() < 2)
		{
			return;
		};
		_ptr->terminal().edit_file(_ts[1]);

		return;
	};

public:

	Emulator() :
		ecpu_{ &direct_memory_ },
		terminal_{ tout_.rdbuf(), tin_.rdbuf() }
	{
		this->insert_command("do", &cmd_do);
		this->insert_command("load", &cmd_load);
		this->insert_command("pause", &cmd_pause);
		this->insert_command("resume", &cmd_resume);
		this->insert_command("mdump", &cmd_mdump);
		this->insert_command("step", &cmd_step);

		this->insert_command("as", &cmd_as);
		this->insert_command("edit", &cmd_edit);

	};

private:
	
	std::unordered_map<std::string, CommandFunction> commands_{};

	bool run_cpu_ = false;

	std::ostringstream tout_{};
	std::istringstream tin_{};

	em::Memory direct_memory_{};
	em::Terminal terminal_;
	em::CPU ecpu_;

	EnvironmentVars env_{};

};

void Emulator::update()
{
	if (this->run_cpu_)
	{
		this->ecpu().step();
	};
};

// Peripheral IO registers start at address 2^15
constexpr size_t MAX_EXECUTABLE_SIZE = 32767;

bool Emulator::load_file(const std::filesystem::path& _path)
{
	auto _data = em::load_x69_machine_code(_path);
	if (!_data)
	{
		return false;
	};

	// max mem size of 65,535

	if (_data->size() > MAX_EXECUTABLE_SIZE)
	{
		return false;
	};
	std::copy(_data->begin(), _data->end(), this->memory().begin());
};

void Emulator::start(uint16_t _atAddress)
{
	this->run_cpu_ = true;
	this->ecpu().special_regs()[em::SpecialRegisters::PC] = _atAddress;
};
void Emulator::start()
{
	this->start(0);
};

void Emulator::pause()
{
	this->run_cpu_ = false;
};
void Emulator::resume()
{
	this->run_cpu_ = true;
};

void Emulator::step()
{
	this->ecpu().step();
};

void Emulator::load_environment(const std::filesystem::path& _fromPath)
{
	this->env_ = load_environment_file(_fromPath);
};
void Emulator::load_environment()
{
	this->env_ = load_environment_file();
};






void Emulator::dump_memory()
{
	auto _path = std::filesystem::current_path();
	_path.append("mem_dump.bin");
	
	if (std::filesystem::exists(_path))
		std::filesystem::remove(_path);
	
	std::ofstream _memDump{ _path, std::ofstream::binary };

	for (auto& b : this->memory())
	{
		_memDump << b;
	};

	_memDump.close();


};

void Emulator::process_command(const std::string& _input)
{
	auto _findSpace = _input.find(' ');
	if (_findSpace == std::string::npos)
	{
		_findSpace = _input.size();
	};

	auto _token = _input.substr(0, _findSpace);

	auto _goodCommand = this->has_command(_token);
	if (_goodCommand)
	{
		this->call_command(_token, _input);
	};

};

void run_emulator(Emulator* _emu)
{

	bool _keepRunning = true;

	while (_keepRunning)
	{
		_emu->update();

		while (_emu->terminal().has_message())
		{
			using MSG_E = em::Terminal::Message::MESSAGE_TYPE_E;

			auto _msg = _emu->terminal().next_message();
			switch (_msg.index())
			{
			case MSG_E::EXITED:
				_keepRunning = false;
				break;
			case MSG_E::GOT_INPUT:
				_emu->process_command(_msg.get<MSG_E::GOT_INPUT>().str_);
				break;
			case MSG_E::STEP:
				_emu->step();
				break;
			default:
				abort();
			};

		};

	};



};



int imain(int argc, char* argv[], char* envp[])
{
	Emulator emu_{};
	load_terminal_pf(&emu_.memory());

	auto _gotPeriphs = x69::emu::parse_peripherals_file("pflayout.txt");
	if (!_gotPeriphs)
	{
		return -2;
	};

	for (auto& _p : _gotPeriphs->peripherals)
	{
		auto _pf = x69::emu::open_peripheral_lib(_p.path);
		if (_pf)
		{
			emu_.memory().register_peripheral(std::move(*_pf), _p.address);
		}
		else
		{
			std::cout << "Failed to load peripheral : " << _p.path << '\n';
		};

	};

	auto _goodOpen = emu_.terminal().open();
	if (!_goodOpen)
	{
		return -1;
	};

	std::cout << "x69 Emulator v0.0.1\n";

	std::vector<std::string> _args{};
	for (int i = 1; i < argc; ++i)
	{
		std::cout << argv[i] << '\n';
		_args.push_back(argv[i]);
	};

	std::filesystem::path _bootFile{ SOURCE_ROOT "executable/out.o" };

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
					std::cout << "Bad Arguement : " << _opt << '\n';
					abort();
				};
			};

		};
	};

	run_emulator(&emu_);

	std::cout << emu_.ecpu().registers() << '\n';
	std::cout << emu_.ecpu().special_regs() << '\n';

	return 0;
};

int main(int argc, char* argv[], char* envp[])
{	
	return imain(argc, argv, envp);
};
