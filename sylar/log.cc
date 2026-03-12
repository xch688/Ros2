#include "log.h"
#include "sylar/log.h"

#include "singleton.h"
#include "util.h"

#include <algorithm>
#include <cstdarg>
#include <functional>
#include <iostream>
#include <map>
#include <sys/time.h>

namespace sylar {

const char* LogLevel::toString(LogLevel::Level level)
{
    switch (level) {
#define XX(name) \
    case LogLevel::Level::name: return #name; break;
        XX(DEBUG);
        XX(INFO);
        XX(WARN);
        XX(ERROR);
        XX(FATAL);
#undef XX
        default: return "UNKNOWN";
    }

    return "UNKNOWN";
}

LogLevel::Level LogLevel::fromString(std::string str)
{
    std::transform(str.begin(), str.end(), str.begin(), ::toupper);
#define XX(level)                      \
    if (str == #level) {               \
        return LogLevel::Level::level; \
    }

    XX(DEBUG);
    XX(INFO);
    XX(WARN);
    XX(ERROR);
    XX(FATAL);

#undef XX

    return LogLevel::Level::UNKNOWN;
}

}   // namespace sylar


namespace sylar {

LogEvent::LogEvent(std::shared_ptr<Logger> logger, LogLevel::Level level, const char* file,
                   int32_t line, uint32_t elapse, uint32_t threadId, uint32_t fiberId,
                   const std::string& threadName)
    : file_(file)
    , line_(line)
    , elapse_(elapse)
    , threadId_(threadId)
    , fiberId_(fiberId)
    , threadName_(threadName)
    , logger_(logger)
    , level_(level)
{
    if (gettimeofday(&time_, nullptr)) {
        std::cerr << "Failed to get time-of-time" << std::endl;
    }
}

void LogEvent::format(const char* fmt, ...)
{
    va_list al;
    va_start(al, fmt);
    format(fmt, al);
    va_end(al);
}

void LogEvent::format(const char* fmt, va_list ap)
{
    char* buf = nullptr;
    if (const int len = vasprintf(&buf, fmt, ap); len != -1) {
        ss_ << std::string(buf, len);
        free(buf);
    }
}

}   // namespace sylar


namespace sylar {

LogEventWrap::LogEventWrap(LogEvent::ptr ev)
    : event_(ev)
{}

LogEventWrap::~LogEventWrap()
{
    event_->getLogger()->log(event_->getLevel(), event_);
}

}   // namespace sylar


namespace sylar {
class MessageFormatItem final : public LogFormatter::FormatItem {
public:
    explicit MessageFormatItem(const std::string& msg = "") {};
    void format(std::ostream& os, std::shared_ptr<Logger> logger, LogLevel::Level level,
                LogEvent::ptr event) override
    {
        os << event->getContent();
    }
};


class SecondFormatItem final : public LogFormatter::FormatItem {
public:
    explicit SecondFormatItem(const std::string& msg = "") {};
    void format(std::ostream& os, std::shared_ptr<Logger> logger, LogLevel::Level level,
                LogEvent::ptr event) override
    {
        int64_t second = event->getTime().tv_sec;
        struct tm tm;
        localtime_r(&second, &tm);
        char buffer[128] = {0};
        snprintf(buffer,
                 sizeof(buffer) - 1,
                 "%04d-%02d-%02d %02d:%02d:%02d",
                 tm.tm_year + 1900,
                 tm.tm_mon + 1,
                 tm.tm_mday,
                 tm.tm_hour,
                 tm.tm_min,
                 tm.tm_sec);

        os << buffer;
    }
};

class MillisecondFormatItem final : public LogFormatter::FormatItem {
public:
    explicit MillisecondFormatItem(const std::string& msg = "") {};
    void format(std::ostream& os, std::shared_ptr<Logger> logger, LogLevel::Level level,
                LogEvent::ptr event) override
    {
        os << event->getTime().tv_usec;
    }
};

class LevelFormatItem final : public LogFormatter::FormatItem {
public:
    explicit LevelFormatItem(const std::string& msg = "") {};
    void format(std::ostream& os, std::shared_ptr<Logger> logger, LogLevel::Level level,
                LogEvent::ptr event) override
    {
        os << LogLevel::toString(level);
    }
};


class ElapseFormatItem final : public LogFormatter::FormatItem {
public:
    explicit ElapseFormatItem(const std::string& msg = "") {};
    void format(std::ostream& os, std::shared_ptr<Logger> logger, LogLevel::Level level,
                LogEvent::ptr event) override
    {
        os << event->getElapse();
    }
};

class NameFormatItem final : public LogFormatter::FormatItem {
public:
    explicit NameFormatItem(const std::string& msg = "") {};
    void format(std::ostream& os, std::shared_ptr<Logger> logger, LogLevel::Level level,
                LogEvent::ptr event) override
    {
        os << event->getLogger()->getName();
    }
};

class ThreadIdFormatItem final : public LogFormatter::FormatItem {
public:
    explicit ThreadIdFormatItem(const std::string& msg = "") {};
    void format(std::ostream& os, std::shared_ptr<Logger> logger, LogLevel::Level level,
                LogEvent::ptr event) override
    {
        os << event->getThreadId();
    }
};


class FiberIdFormatItem final : public LogFormatter::FormatItem {
public:
    explicit FiberIdFormatItem(const std::string& msg = "") {};
    void format(std::ostream& os, std::shared_ptr<Logger> logger, LogLevel::Level level,
                LogEvent::ptr event) override
    {
        os << event->getFiberId();
    }
};

class ThreadNameFormatItem final : public LogFormatter::FormatItem {
public:
    explicit ThreadNameFormatItem(const std::string& msg = "") {};
    void format(std::ostream& os, std::shared_ptr<Logger> logger, LogLevel::Level level,
                LogEvent::ptr event) override
    {
        os << event->getThreadName();
    }
};

class FileNameFormatItem final : public LogFormatter::FormatItem {
public:
    explicit FileNameFormatItem(const std::string& msg = "") {};
    void format(std::ostream& os, std::shared_ptr<Logger> logger, LogLevel::Level level,
                LogEvent::ptr event) override
    {
        os << event->getFile();
    }
};

class LineFormatItem final : public LogFormatter::FormatItem {
public:
    explicit LineFormatItem(const std::string& msg = "") {};
    void format(std::ostream& os, std::shared_ptr<Logger> logger, LogLevel::Level level,
                LogEvent::ptr event) override
    {
        os << event->getLine();
    }
};


class NewLineFormatItem final : public LogFormatter::FormatItem {
public:
    explicit NewLineFormatItem(const std::string& msg = "") {};
    void format(std::ostream& os, std::shared_ptr<Logger> logger, LogLevel::Level level,
                LogEvent::ptr event) override
    {
        os << std::endl;
    }
};

class StringFormatItem final : public LogFormatter::FormatItem {
public:
    explicit StringFormatItem(std::string msg = "")
        : msg_(std::move(msg)) {};
    void format(std::ostream& os, std::shared_ptr<Logger> logger, LogLevel::Level level,
                LogEvent::ptr event) override
    {
        os << msg_;
    }

private:
    std::string msg_;
};

class TabFormatItem final : public LogFormatter::FormatItem {
public:
    explicit TabFormatItem(const std::string& msg = "") {};
    void format(std::ostream& os, std::shared_ptr<Logger> logger, LogLevel::Level level,
                LogEvent::ptr event) override
    {
        os << "  ";
    }
};

}   // namespace sylar



namespace sylar {

LogFormatter::LogFormatter(const std::string& fmt)
    : pattern_(fmt)
{
    init();
}

void LogFormatter::init()
{
    // "%d{%Y-%m-%d %H:%M:%S}%T%t%T%N%T%F%T[%p]%T[%c]%T%f:%l%T%m%n"
    std::string pattern = pattern_;
    std::vector<std::pair<std::string, int32_t>> items;
    while (!pattern.empty()) {
        std::string::size_type percentSign = pattern.find_first_of('%');
        if ((percentSign == std::string::npos) || (percentSign == pattern.size() - 1)) {
            items.emplace_back(pattern, 0);
            break;
        }

        if (isalpha(pattern[percentSign + 1])) {
            if (percentSign != 0) {
                items.emplace_back(pattern.substr(0, percentSign), 0);
            }
            items.emplace_back(pattern.substr(percentSign + 1, 1), 1);
        }
        else {
            items.emplace_back(pattern.substr(0, percentSign + 2), 1);
        }
        pattern = pattern.erase(0, percentSign + 2);
    }

    static std::map<std::string, std::function<FormatItem::ptr(const std::string& str)>>
        s_formatItems = {
#define XX(str, C)                                            \
    {                                                         \
        #str, [](const std::string& fmt) -> FormatItem::ptr { \
            return FormatItem::ptr(std::make_shared<C>(fmt)); \
        }                                                     \
    }
            XX(m, MessageFormatItem),
            XX(S, SecondFormatItem),
            XX(M, MillisecondFormatItem),
            XX(p, LevelFormatItem),
            XX(r, ElapseFormatItem),
            XX(c, NameFormatItem),
            XX(t, ThreadIdFormatItem),
            XX(n, NewLineFormatItem),
            XX(f, FileNameFormatItem),
            XX(l, LineFormatItem),
            XX(T, TabFormatItem),
            XX(F, FiberIdFormatItem),
            XX(N, ThreadNameFormatItem),
#undef XX
        };

    for (auto& i : items) {
        if (i.second == 0) {
            items_.emplace_back(new StringFormatItem(i.first));
        }
        else {
            if (const auto it = s_formatItems.find(i.first); it == s_formatItems.end()) {
                items_.emplace_back(new StringFormatItem("<<error_format %" + i.first + ">>"));
                error_ = true;
            }
            else {
                items_.emplace_back(it->second(i.first));
            }
        }
    }
    // while (!pattern.empty()) {}
}   // namespace sylar

std::string LogFormatter::format(std::shared_ptr<Logger> logger, LogLevel::Level level,
                                 LogEvent::ptr event)
{
    std::stringstream ss;
    for (const auto& item : items_) {
        item->format(ss, logger, level, event);
    }

    return ss.str();
}

std::ostream& LogFormatter::format(std::ostream& os, std::shared_ptr<Logger> logger,
                                   LogLevel::Level level, LogEvent::ptr event)
{
    for (const auto& item : items_) {
        item->format(os, logger, level, event);
    }

    return os;
}

}   // namespace sylar


namespace sylar {

void LogAppender::setFormatter(LogFormatter::ptr val)
{
    Mutex_t::Lock lock(mutex_);
    formatter_ = val;
    hasFormatter_ = !!formatter_;
}

LogFormatter::ptr LogAppender::getFormatter()
{
    Mutex_t::Lock lock(mutex_);
    return formatter_;
}

}   // namespace sylar


namespace sylar {

Logger::Logger(const std::string& name)
    : name_(name)
    , level_(LogLevel::Level::DEBUG)
{
    formatter_ = std::make_shared<LogFormatter>("%S.%M%T%t%T[%p]%T%f:%l%T%m%n");
}

void Logger::log(LogLevel::Level level, LogEvent::ptr event)
{
    // 若日志器无appender, 则将日志事件传递给root日志器
    if (level >= level_) {
        auto self = shared_from_this();
        RWMutex_t::ReadLock lock(mutex_);
        if (!appenders_.empty()) {
            for (auto& i : appenders_) {
                i->log(self, level, event);
            }
        }
        else if (root_) {
            root_->log(level, event);
        }
    }
}

void Logger::addAppender(LogAppender::ptr appender)
{
    RWMutex_t::WriteLock lock(mutex_);
    if (!appender->getFormatter()) {
        LogAppender::Mutex_t::Lock ll(appender->mutex_);
        appender->formatter_ = formatter_;
    }
    appenders_.emplace_back(appender);
}

void Logger::delAppender(LogAppender::ptr appender)
{
    RWMutex_t::WriteLock lock(mutex_);
    auto it = std::find(appenders_.begin(), appenders_.end(), appender);
    if (it != appenders_.end()) {
        appenders_.erase(it);
    }
}

void Logger::clearAppenders()
{
    RWMutex_t::WriteLock lock(mutex_);
    appenders_.clear();
}

void Logger::setFormatter(LogFormatter::ptr val)
{
    RWMutex_t::WriteLock lock(mutex_);
    formatter_ = val;
    for (auto& i : appenders_) {
        LogAppender::Mutex_t::Lock ll(i->mutex_);
        if (!i->hasFormatter_) {
            i->formatter_ = formatter_;
            i->hasFormatter_ = true;
        }
    }
}

void Logger::setFormatter(const std::string& val)
{
    sylar::LogFormatter::ptr new_val = std::make_shared<sylar::LogFormatter>(val);
    if (new_val->isError()) {
        std::cout << "Logger setFormatter name=" << name_ << " value=" << val
                  << " invalid formatter" << std::endl;
        return;
    }
    setFormatter(new_val);
}

LogFormatter::ptr Logger::getFormatter()
{
    RWMutex_t::ReadLock lock(mutex_);
    return formatter_;
}

std::string Logger::toYamlString()
{
    return "";
}

bool Logger::reopen()
{
    RWMutex_t::ReadLock lock(mutex_);
    auto appenders = appenders_;
    lock.unlock();

    for (auto& i : appenders) {
        i->reopen();
    }

    return true;
}

}   // namespace sylar


namespace sylar {

void StdoutLogAppender::log(Logger::ptr logger, LogLevel::Level level, LogEvent::ptr event)
{
    if (level >= level_) {
        Mutex_t::Lock lock(mutex_);
        formatter_->format(std::cout, logger, level, event);
    }
}

std::string StdoutLogAppender::toYamlString()
{
    return "";
}

}   // namespace sylar


namespace sylar {

FileLogAppender::FileLogAppender(const std::string& filename)
    : filename_(filename)
{
    FileLogAppender::reopen();   // 强制指定调用的函数版本
}

void FileLogAppender::log(Logger::ptr logger, LogLevel::Level level, LogEvent::ptr event)
{
    if (level >= level_) {
        uint64_t now = event->getTime().tv_sec;
        constexpr int32_t ONE_SECOND = 1;
        if (now >= lastTime_ + 3 * ONE_SECOND) {
            reopen();
            lastTime_ = now;
        }

        Mutex_t::Lock lock(mutex_);
        if (!formatter_->format(filestream_, logger, level, event)) {
            std::cerr << "file log appender write log log error." << std::endl;
        }
    }
}

std::string FileLogAppender::toYamlString()
{
    return "";
}

bool FileLogAppender::reopen()
{
    Mutex_t::Lock lock(mutex_);
    if (filestream_) {
        filestream_.close();
    }

    return FSUtils::OpenForWrite(filestream_, filename_, std::ios::app);
}

}   // namespace sylar


namespace sylar {

LoggerManager::LoggerManager()
{
    root_ = std::make_shared<sylar::Logger>();
    root_->addAppender(std::make_shared<sylar::StdoutLogAppender>());
    loggers_[root_->getName()] = root_;

    init();
}

Logger::ptr LoggerManager::getLogger(const std::string& name)
{
    do {
        Mutex_t::ReadLock lock(mutex_);
        auto it = loggers_.find(name);
        if (it != loggers_.end()) {
            return it->second;
        }
    } while (false);

    Mutex_t::ReadLock lock(mutex_);
    auto it = loggers_.find(name);
    if (it != loggers_.end()) {
        return it->second;
    }

    Logger::ptr logger = std::make_shared<sylar::Logger>(name);
    logger->root_ = root_;
    loggers_[name] = logger;

    return logger;
}

void LoggerManager::init() {}

std::string LoggerManager::toYamlString()
{
    return "";
}

bool LoggerManager::reopen()
{
    Mutex_t::ReadLock lock(mutex_);
    auto loggers = loggers_;
    auto root = root_;
    lock.unlock();

    root->reopen();
    for (auto& i : loggers) {
        i.second->reopen();
    }

    return true;
}

using LoggerMgr = sylar::Singleton<LoggerManager>;

}   // namespace sylar