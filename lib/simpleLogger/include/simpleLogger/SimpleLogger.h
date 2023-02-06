#pragma once

#include <iostream>
#include <memory>

class SimpleLogger
{
public:
	using ostreamManipulator = std::ostream &(*)(std::ostream &);
	using basicIosManipulator = std::basic_ios<char> &(*)(std::basic_ios<char> &);
	using iosBaseManipulator = std::ios_base &(*)(std::ios_base &);

	enum class Level : unsigned int
	{
		off = 0,
		critical = 1,
		error = 2,
		warning = 3,
		info = 4,
		debug = 5,
	};

	class OstreamWrapper
	{
	public:
		OstreamWrapper(SimpleLogger &logger, SimpleLogger::Level level) : logger_{logger}, level_{level} {}
		template <typename T>
		OstreamWrapper &operator<<(const T &x)
		{
			if (static_cast<unsigned int>(logger_.getLevel()) >= static_cast<unsigned int>(level_))
			{
				logger_ << x;
			}
			return *this;
		}

		OstreamWrapper &operator<<(ostreamManipulator om) { return operator<<<ostreamManipulator>(om); }
		OstreamWrapper &operator<<(basicIosManipulator bim) { return operator<<<basicIosManipulator>(bim); }
		OstreamWrapper &operator<<(iosBaseManipulator ibm) { return operator<<<iosBaseManipulator>(ibm); }

	private:
		SimpleLogger &logger_;
		SimpleLogger::Level level_{SimpleLogger::Level::warning};
	};

	// Singleton accessor
	static SimpleLogger &get();

	template <typename T>
	SimpleLogger &operator<<(const T &x)
	{
		if (stream_ != nullptr)
			*stream_ << x;
		return *this;
	}

	SimpleLogger &operator<<(ostreamManipulator om) { return operator<<<ostreamManipulator>(om); }
	SimpleLogger &operator<<(basicIosManipulator bim) { return operator<<<basicIosManipulator>(bim); }
	SimpleLogger &operator<<(iosBaseManipulator ibm) { return operator<<<iosBaseManipulator>(ibm); }

	Level getLevel() { return level_; }

	OstreamWrapper critical;
	OstreamWrapper error;
	OstreamWrapper warning;
	OstreamWrapper info;
	OstreamWrapper debug;

private:
	SimpleLogger(std::ostream *stream, Level level)
	    : critical(*this, Level::critical), error(*this, Level::error), warning(*this, Level::warning),
	      info(*this, Level::info), debug(*this, Level::debug), stream_{stream}, level_{level}
	{
	}


	std::ostream *stream_{nullptr};
	Level level_{Level::warning};
};

#define simpleLogger SimpleLogger::get()
// #define SETUP_SIMPLE_LOGGER(simpleLoggerName)                                                                          \
// 	static SimpleLogger simpleLoggerName(&std::cout, SimpleLogger::Level::SIMPLE_LOGGER_LEVEL)
