#include<iostream>
#include<iomanip>
#include<sstream>
#include<fstream>
#include<ctime>
#include "SimpleLog.h"

using namespace std;

//***********************************************************************************************************************/
//***																													*/
//***  CLASS SimpleLog																									*/
//***																													*/
//***********************************************************************************************************************
SimpleLog::SimpleLog()
{
	m_DefaultFilter = new DefaultFilter();
	m_DefaultWriter = new DefaultWriter();
	m_Filter = m_DefaultFilter;
	m_Writer = m_DefaultWriter;
}

SimpleLog::SimpleLog(IFilter *filter)
{
    m_DefaultFilter = nullptr;
	m_DefaultWriter = new DefaultWriter();
	m_Filter = filter;
	m_Writer = m_DefaultWriter;
}

SimpleLog::SimpleLog(IWriter* writer)
{
	m_DefaultFilter = new DefaultFilter();
	m_DefaultWriter = nullptr;
	m_Filter = m_DefaultFilter;
	m_Writer = writer;
}

SimpleLog::SimpleLog(IFilter *filter, IWriter *writer)
{
    m_DefaultFilter = nullptr;
	m_DefaultWriter = nullptr;
    m_Filter = filter;
    m_Writer = writer;
}

SimpleLog::~SimpleLog()
{
	Flush();
    if(m_DefaultFilter != nullptr) delete m_DefaultFilter;
	if(m_DefaultWriter != nullptr) delete m_DefaultWriter;
}

void SimpleLog::SetFilter(IFilter *filter)
{
    m_Filter = filter;
}

void SimpleLog::SetWriter(IWriter *writer)
{
    m_Writer = writer;
}

void SimpleLog::Log(SimpleLog::Level level, const string& message, const string& module, int line, const string& function)
{
	size_t found;
	string onlyModule;

	found = module.find_last_of("/\\");
	onlyModule = module.substr(found + 1);

	if (m_Filter->Filter(level, message, onlyModule, line, function)) m_Writer->Writer(level, message, onlyModule, line, function);
}

SimpleLog::Line SimpleLog::LogOut(SimpleLog* log, SimpleLog::Level level, const string& module, int line, const string& function)
{
	return Line(log, level, module, line, function);
}

void SimpleLog::Flush()
{
	m_Writer->Flush();
}

string SimpleLog::Level2String(SimpleLog::Level level)
{
	switch(level)
	{
		case SimpleLog::LVL_FATAL :
			return "FATAL";
			break;
		case SimpleLog::LVL_ERROR :
			return "ERROR";
			break;
		case SimpleLog::LVL_WARNING :
			return "WARNING";
			break;
		case SimpleLog::LVL_INFO :
			return "INFO";
			break;
		case SimpleLog::LVL_DEBUG :
			return "DEBUG";
			break;
		case SimpleLog::LVL_VERBOSE:
			return "VERBOSE";
			break;
		case SimpleLog::LVL_TRACE:
			return "TRACE";
			break;
	}
	return "UNKNOW";
}

//***********************************************************************************************************************/
//***																													*/
//***  CLASS SimpleLog::Line																							*/
//***																													*/
//***********************************************************************************************************************
SimpleLog::Line::Line(SimpleLog* log, SimpleLog::Level level, const std::string& module, int line, const std::string& function) : m_log(log), m_level(level), m_module(module), m_line(line), m_function(function)
{
	m_message = new std::ostringstream();
}

SimpleLog::Line::~Line()
{
	m_log->Log(m_level, m_message->str(), m_module, m_line, m_function);
	delete m_message;
}

//***********************************************************************************************************************/
//***																													*/
//***  CLASS DefaultFilter																								*/
//***																													*/
//***********************************************************************************************************************
SimpleLog::DefaultFilter::DefaultFilter() : m_module(""), m_function("")
{
    #ifdef DEBUG
	m_level = SimpleLog::LVL_VERBOSE;
	#else
	m_level = SimpleLog::LVL_ERROR;
	#endif
}

SimpleLog::DefaultFilter::DefaultFilter(SimpleLog::Level level) : m_level(level), m_module(""), m_function("")
{
}

SimpleLog::DefaultFilter::DefaultFilter(SimpleLog::Level level, const std::string& module, const std::string& function) : m_level(level), m_module(module), m_function(function)
{
}

SimpleLog::DefaultFilter::~DefaultFilter()
{

}

void SimpleLog::DefaultFilter::SetLevel(SimpleLog::Level level)
{
	m_level = level;
}

void SimpleLog::DefaultFilter::SetModule(const std::string& module)
{
	m_module = module;
}

void SimpleLog::DefaultFilter::SetFunction(const std::string& function)
{
	m_function = function;
}

#ifndef _MSC_VER
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
#endif

bool SimpleLog::DefaultFilter::Filter(SimpleLog::Level level, const std::string& message, const std::string& module, int line, const std::string& function)
{
	bool result;

	if (level > m_level) return false;

	result = true;
	if ((m_module != "") && (m_module != module)) result = false;
	if ((m_function != "") && (m_function != function)) result = false;
	return result;
}

#ifndef _MSC_VER
#pragma GCC diagnostic pop
#endif

//***********************************************************************************************************************/
//***																													*/
//***  CLASS DefaultWriter																								*/
//***																													*/
//***********************************************************************************************************************
SimpleLog::DefaultWriter::DefaultWriter()
{
	m_stream = &cout;
}

SimpleLog::DefaultWriter::DefaultWriter(std::ostream &stream)
{
	m_stream = &stream;
}

SimpleLog::DefaultWriter::~DefaultWriter()
{
}

void SimpleLog::DefaultWriter::SetStream(std::ostream &stream)
{
	m_stream = &stream;
}

void SimpleLog::DefaultWriter::Writer(SimpleLog::Level level, const std::string& message, const std::string& module, int line, const std::string& function)
{
	ostringstream oss;
	ofstream logFile;
	time_t internToday = time(0);
	struct tm localToday;

	#ifdef _MSC_VER
    localtime_s(&localToday, &internToday);
	#else
	//localtime_r(&localToday, &internToday);
    localToday = *localtime(&internToday);
	#endif

	oss << localToday.tm_year + 1900 << '-' << setw(2) << setfill('0') << localToday.tm_mon + 1 << '-' << setw(2) << setfill('0') << localToday.tm_mday << ' ';
	oss << setw(2) << setfill('0') << localToday.tm_hour << ':' << setw(2) << setfill('0') << localToday.tm_min << ':' << setw(2) << setfill('0') << localToday.tm_sec << ' ';
	oss << SimpleLog::Level2String(level) << " " << module << " l." << line << " " << function << " " << message << endl;

	*m_stream << oss.str();

	return;
}

void SimpleLog::DefaultWriter::Flush()
{
	return;
}
