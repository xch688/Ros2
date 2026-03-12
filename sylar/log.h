#pragma once

#include "sylar/mutex.h"
#include "sylar/singleton.h"
#include "sylar/util.h"

#include <fstream>
#include <list>
#include <map>
#include <memory>
#include <sstream>
#include <string>
#include <vector>

namespace sylar {

#define SYLAR_LOG_LEVEL(logger, level)                                                 \
    if (logger->getLevel() <= level)                                                   \
    sylar::LogEventWrap(sylar::LogEvent::ptr(new sylar::LogEvent(logger,               \
                                                                 level,                \
                                                                 __FILE__,             \
                                                                 __LINE__,             \
                                                                 0,                    \
                                                                 sylar::GetThreadId(), \
                                                                 sylar::GetFiberId(),  \
                                                                 "workThread")))       \
        .getSS()   //

#define SYLAR_LOG_DEBUG(logger) SYLAR_LOG_LEVEL(logger, sylar::LogLevel::DEBUG)
#define SYLAR_LOG_INFO(logger) SYLAR_LOG_LEVEL(logger, sylar::LogLevel::INFO)
#define SYLAR_LOG_WARN(logger) SYLAR_LOG_LEVEL(logger, sylar::LogLevel::WARN)
#define SYLAR_LOG_ERROR(logger) SYLAR_LOG_LEVEL(logger, sylar::LogLevel::ERROR)
#define SYLAR_LOG_FATAL(logger) SYLAR_LOG_LEVEL(logger, sylar::LogLevel::FATAL)


#define SYLAR_LOG_FMT_LEVEL(logger, level, fmt, ...)                                       \
    if (logger->getLevel() <= level)                                                       \
        sylar::LogEventWrap(sylar::LogEvent::ptr(new sylar::LogEvent(logger,               \
                                                                     level,                \
                                                                     __FILE__,             \
                                                                     __LINE__,             \
                                                                     0,                    \
                                                                     sylar::GetThreadId(), \
                                                                     sylar::GetFiberId(),  \
                                                                     "workThread")))       \
            .getEvent()                                                                    \
            ->format(fmt, ##__VA_ARGS__);


#define SYLAR_LOG_FMT_DEBUG(logger, fmt, ...) \
    SYLAR_LOG_FMT_LEVEL(logger, sylar::LogLevel::Level::DEBUG, fmt, ##__VA_ARGS__)
#define SYLAR_LOG_FMT_INFO(logger, fmt, ...) \
    SYLAR_LOG_FMT_LEVEL(logger, sylar::LogLevel::Level::INFO, fmt, ##__VA_ARGS__)
#define SYLAR_LOG_FMT_WARN(logger, fmt, ...) \
    SYLAR_LOG_FMT_LEVEL(logger, sylar::LogLevel::Level::WARN, fmt, ##__VA_ARGS__)
#define SYLAR_LOG_FMT_ERROR(logger, fmt, ...) \
    SYLAR_LOG_FMT_LEVEL(logger, sylar::LogLevel::Level::ERROR, fmt, ##__VA_ARGS__)
#define SYLAR_LOG_FMT_FATAL(logger, fmt, ...) \
    SYLAR_LOG_FMT_LEVEL(logger, sylar::LogLevel::Level::FATAL, fmt, ##__VA_ARGS__)


#define SYLAR_LOG_ROOT() sylar::LoggerMgr::GetInstance().getRoot()
#define SYLAR_LOG_NAME(name) sylar::LoggerMgr::GetInstance().getLogger(name)


class Logger;
class LoggerManager;

class LogLevel {
public:
    enum Level
    {
        UNKNOWN = 0,
        DEBUG = 1,
        INFO = 2,
        WARN = 3,
        ERROR = 4,
        FATAL = 5,
    };

    static const char* toString(LogLevel::Level level);
    static LogLevel::Level fromString(std::string str);
};


class LogEvent {
public:
    using ptr = std::shared_ptr<LogEvent>;

public:
    LogEvent(std::shared_ptr<Logger> logger, LogLevel::Level level, const char* file, int32_t line,
             uint32_t elapse, uint32_t threadId, uint32_t fiberId, const std::string& threadName);

    const char* getFile() const { return file_; }
    int32_t getLine() const { return line_; }
    uint32_t getElapse() const { return elapse_; }
    uint32_t getThreadId() const { return threadId_; }
    uint32_t getFiberId() const { return fiberId_; }
    struct timeval getTime() const { return time_; }
    const std::string& getThreadName() const { return threadName_; }
    std::string getContent() const { return ss_.str(); }
    std::shared_ptr<Logger> getLogger() const { return logger_; }
    LogLevel::Level getLevel() const { return level_; }

    std::stringstream& getSS() { return ss_; }
    void format(const char* fmt, ...);
    void format(const char* fmt, va_list ap);

private:
    const char* file_ = nullptr;
    int32_t line_ = 0;
    uint32_t elapse_ = 0;   // 程序启动到现在的毫秒数
    uint32_t threadId_ = 0;
    uint32_t fiberId_ = 0;
    struct timeval time_{};   // 时间戳
    std::string threadName_;

    std::stringstream ss_;   // 日志内容流
    std::shared_ptr<Logger> logger_;
    LogLevel::Level level_{LogLevel::UNKNOWN};
};


class LogEventWrap {
public:
    explicit LogEventWrap(LogEvent::ptr ev);
    ~LogEventWrap();
    LogEvent::ptr getEvent() const { return event_; }
    std::stringstream& getSS() { return event_->getSS(); }

private:
    LogEvent::ptr event_;
};


class LogFormatter {
public:
    using ptr = std::shared_ptr<LogFormatter>;

public:
    class FormatItem {
    public:
        using ptr = std::shared_ptr<FormatItem>;
        virtual ~FormatItem() = default;

        virtual void format(std::ostream& os, std::shared_ptr<Logger> logger, LogLevel::Level level,
                            LogEvent::ptr event) = 0;
    };

public:
    explicit LogFormatter(const std::string& fmt);

    void init();
    [[nodiscard]] bool isError() const { return error_; }
    [[nodiscard]] const std::string& getPattern() const { return pattern_; }

    std::string format(std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event);
    std::ostream& format(std::ostream& os, std::shared_ptr<Logger> logger, LogLevel::Level level,
                         LogEvent::ptr event);

private:
    std::string pattern_;
    std::vector<FormatItem::ptr> items_;
    bool error_ = {false};
};

class LogAppender {
private:
    friend class Logger;

public:
    using ptr = std::shared_ptr<LogAppender>;
    using Mutex_t = Spinlock;

public:
    virtual ~LogAppender() = default;
    virtual void log(std::shared_ptr<Logger> logger, LogLevel::Level level,
                     LogEvent::ptr event) = 0;
    virtual std::string toYamlString() = 0;
    virtual bool reopen() { return true; }

    void setFormatter(LogFormatter::ptr val);
    LogFormatter::ptr getFormatter();
    [[nodiscard]] LogLevel::Level getLevel() const { return level_; }
    void setLevel(const LogLevel::Level level) { level_ = level; }

protected:
    LogLevel::Level level_ = LogLevel::Level::DEBUG;
    bool hasFormatter_ = {false};
    Mutex_t mutex_;
    LogFormatter::ptr formatter_;
};


class Logger : public std::enable_shared_from_this<Logger> {
private:
    friend class LoggerManager;

public:
    using ptr = std::shared_ptr<Logger>;
    using RWMutex_t = RWSpinlock;

public:
    explicit Logger(const std::string& name = "root");
    void log(LogLevel::Level level, LogEvent::ptr event);

    void addAppender(LogAppender::ptr appender);
    void delAppender(LogAppender::ptr appender);
    void clearAppenders();

    LogLevel::Level getLevel() const { return level_; }
    void setLevel(LogLevel::Level level) { level_ = level; }
    const std::string& getName() const { return name_; }

    void setFormatter(LogFormatter::ptr val);
    void setFormatter(const std::string& val);
    LogFormatter::ptr getFormatter();

    std::string toYamlString();
    bool reopen();

private:
    std::string name_;   // 日志器名称
    LogLevel::Level level_;
    RWMutex_t mutex_;
    std::list<LogAppender::ptr> appenders_;
    LogFormatter::ptr formatter_;
    Logger::ptr root_;   // 根日志器
};


class StdoutLogAppender : public LogAppender {
public:
    using ptr = std::shared_ptr<StdoutLogAppender>;
    void log(Logger::ptr logger, LogLevel::Level level, LogEvent::ptr event) override;
    std::string toYamlString() override;
};


class FileLogAppender : public LogAppender {
public:
    using ptr = std::shared_ptr<FileLogAppender>;

public:
    explicit FileLogAppender(const std::string& filename);
    void log(Logger::ptr logger, LogLevel::Level level, LogEvent::ptr event) override;
    std::string toYamlString() override;
    bool reopen() override;

private:
    std::string filename_;
    std::ofstream filestream_;
    uint64_t lastTime_ = 0;
};


class LoggerManager {
public:
    using Mutex_t = RWSpinlock;

public:
    LoggerManager();
    Logger::ptr getLogger(const std::string& name);

    void init();
    [[nodiscard]] Logger::ptr getRoot() const { return root_; }
    std::string toYamlString();
    bool reopen();

private:
    Mutex_t mutex_;
    std::map<std::string, Logger::ptr> loggers_;
    Logger::ptr root_;   // 主日志器
};

using LoggerMgr = sylar::Singleton<LoggerManager>;

}   // namespace sylar
