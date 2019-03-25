#ifndef __LOG4CPP_H__
#define __LOG4CPP_H__

#include <log4cpp/Category.hh>
#include <log4cpp/FileAppender.hh>
#include <log4cpp/PatternLayout.hh>
#include <log4cpp/OstreamAppender.hh>
#include <iostream>

using namespace std;
using namespace std::__cxx11;

#define LOG(__level) log4cpp::Category::getRoot() << log4cpp::Priority::__level << __FILE__ << " " << __LINE__ << ":"

extern log4cpp::Appender *appender;
extern log4cpp::PatternLayout *patternLayout;
extern log4cpp::Category &root;


extern void Log4cppInit(string logFileName);

#endif

