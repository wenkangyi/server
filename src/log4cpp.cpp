#include "log4cpp.h"

// using namespace std;
// using namespace std::__cxx11;


log4cpp::Appender *appender = NULL;
log4cpp::PatternLayout *patternLayout = NULL;
log4cpp::Category &root = log4cpp::Category::getRoot();

//初始化log4cpp日志系统

void Log4cppInit(string logFileName)
{
	//out std::out
	//log4cpp::Appender *appender = new log4cpp::OstreamAppender("root",&std::cout);
	//out log file
	appender = new log4cpp::FileAppender("root",logFileName);

	patternLayout = new log4cpp::PatternLayout();
	patternLayout->setConversionPattern("%d [%p] - %m%n");
	appender->setLayout(patternLayout);
	
	root.setPriority(log4cpp::Priority::DEBUG);
	root.addAppender(appender);
}



