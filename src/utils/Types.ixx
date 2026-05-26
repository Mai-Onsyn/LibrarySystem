module;
#include <memory>
#include <mutex>
export module Types;

export using std::unique_ptr;

export typedef std::string String;
export typedef bool Boolean;
export typedef int32_t Int;
export typedef int64_t Long;
export typedef double Double;
export typedef float Float;
export typedef std::mutex Mutex;
export typedef std::condition_variable ConditionVariable;
export typedef std::lock_guard<Mutex> LockGuard;
export typedef std::unique_lock<Mutex> UniqueLock;

export typedef std::runtime_error RuntimeError;
