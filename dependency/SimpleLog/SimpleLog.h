#ifndef SIMPLELOG_H
#define SIMPLELOG_H

#include <sstream>
#include <fstream>
#include <string>
#include <map>
#include <set>
#include <list>
#include <vector>
#include <deque>

#define LOG_FATAL(log)		log->LogOut(log, SimpleLog::LVL_FATAL,	__FILE__, __LINE__, __FUNCTION__)
#define LOG_ERROR(log)		log->LogOut(log, SimpleLog::LVL_ERROR,	__FILE__, __LINE__, __FUNCTION__)
#define LOG_WARNING(log)	log->LogOut(log, SimpleLog::LVL_WARNING,__FILE__, __LINE__, __FUNCTION__)
#define LOG_INFO(log)		log->LogOut(log, SimpleLog::LVL_INFO,	__FILE__, __LINE__, __FUNCTION__)
#define LOG_VERBOSE(log)	log->LogOut(log, SimpleLog::LVL_VERBOSE,__FILE__, __LINE__, __FUNCTION__)
#define LOG_DEBUG(log)		log->LogOut(log, SimpleLog::LVL_DEBUG,	__FILE__, __LINE__, __FUNCTION__)
#define LOG_TRACE(log)		log->LogOut(log, SimpleLog::LVL_TRACE,	__FILE__, __LINE__, __FUNCTION__)

class SimpleLog
{
	public:
		enum Level {LVL_FATAL, LVL_ERROR, LVL_WARNING, LVL_INFO, LVL_VERBOSE, LVL_DEBUG, LVL_TRACE};
		class IFilter;
		class IWriter;
		class Line;
		class DefaultFilter;
		class DefaultWriter;

		SimpleLog();
		SimpleLog(IFilter *filter);
		SimpleLog(IWriter *writer);
		SimpleLog(IFilter *filter, IWriter *writer);
		~SimpleLog();
		void SetFilter(IFilter *filter);
		void SetWriter(IWriter *writer);
		void Log(Level level, const std::string& message, const std::string& module, int line, const std::string& function);
		void Flush();
		Line LogOut(SimpleLog* log, Level level, const std::string& module, int line, const std::string& function);
		static std::string Level2String(Level level);

	private:
		IFilter *m_Filter;
		IWriter *m_Writer;
		DefaultFilter *m_DefaultFilter;
		DefaultWriter *m_DefaultWriter;
};

class SimpleLog::Line
{
	public:
		Line(SimpleLog* log, SimpleLog::Level level, const std::string& module, int line, const std::string& function);
		~Line();
		template<class T>
		inline Line& operator<<(const T &something)
		{
			*m_message << something;
			return *this;
		}

		template<class T1>
		inline Line& operator<<(const std::set<T1> &set);

		template<class T1>
		inline Line& operator<<(const std::multiset<T1> &multiset);

		template<class T1>
		inline Line& operator<<(const std::list<T1> &list);

		template<class T1>
		inline Line& operator<<(const std::vector<T1> &vector);

		template<class T1>
		inline Line& operator<<(const std::deque<T1> &deque);

		template<class T1, class T2>
		inline Line& operator<<(const std::pair<T1, T2> &pair);

		template<class T1, class T2>
		inline Line& operator<<(const std::map<T1, T2> &map);

		template<class T1, class T2>
		inline Line& operator<<(const std::multimap<T1, T2> &multimap);

	private:
		SimpleLog* m_log;
		SimpleLog::Level m_level;
		std::string m_module;
		int m_line;
		std::string m_function;
		std::ostringstream *m_message;
};

class SimpleLog::IFilter
{
	public:
		virtual bool Filter(Level level, const std::string& message, const std::string& module, int Line, const std::string& function) = 0;
};

class SimpleLog::IWriter
{
	public:
		virtual void Writer(Level level, const std::string& message, const std::string& module, int Line, const std::string& function) = 0;
		virtual void Flush() = 0;
};

class SimpleLog::DefaultFilter : public SimpleLog::IFilter
{
	public:
		DefaultFilter();
		DefaultFilter(SimpleLog::Level level);
		DefaultFilter(SimpleLog::Level level, const std::string& module, const std::string& function);
		virtual ~DefaultFilter();
		void SetLevel(SimpleLog::Level level);
		void SetModule(const std::string& module);
		void SetFunction(const std::string& function);
		virtual bool Filter(SimpleLog::Level level, const std::string& message, const std::string& module, int Line, const std::string& function);
	private:
		SimpleLog::Level m_level;
		std::string m_module;
		std::string m_function;
};

class SimpleLog::DefaultWriter : public SimpleLog::IWriter
{
	public:
		DefaultWriter();
		DefaultWriter(std::ostream &stream);
		virtual ~DefaultWriter();
		void SetStream(std::ostream &stream);
		virtual void Writer(SimpleLog::Level level, const std::string& message, const std::string& module, int Line, const std::string& function);
		virtual void Flush();
	private:
		std::ostream *m_stream;
};

#include "SimpleLog.inl"  // spécialisation template dans la classe SimpleLog::Line

#endif
