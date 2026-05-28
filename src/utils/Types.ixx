module;
#include <iostream>
#include <map>
#include <memory>
#include <mutex>
#include <vector>
#include <string>
export module Types;

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
export template<typename T, typename Alloc = std::allocator<T>>
class List : public std::vector<T, Alloc> {
public:
    using std::vector<T, Alloc>::vector;

    template <typename... Args>
    decltype(auto) add(Args&&... args) {
        return this->emplace_back(std::forward<Args>(args)...);
    }
};

export template<typename K, typename V> using Map = std::map<K, V>;

export template<typename T> using UniquePtr = std::unique_ptr<T>;
export template<typename T> using SharedPtr = std::shared_ptr<T>;
export using std::make_unique;
export using std::cin;
export using std::cout;
export using std::print;
export using std::println;
export using std::printf;