

// custom error_category example
#include <iostream>       // std::cout, std::endl
#include <system_error>   // std::is_error_condition_enum, std::error_category,
                          // std::error_condition, std::error_code
#include <string>         // std::string

// custom error enum type:
enum class ModuleErrorCode { 
    Success=0,
    Failed,
    Unknown
};

// std::error_code 的构造函数
// template<class ErrorCodeEnum> error_code(ErrorCodeEnum e) noexcept;
// 会调用 make_error_code(enum_value) 工厂函数，
// 为了避免从任意类型的值都能转换成std::error_code，需要在std命名空间中特化此模版
namespace std {
    template<> 
    struct is_error_code_enum<ModuleErrorCode> : public true_type {};
}

// custom error_category:
class modules_error_category : public std::error_category 
{
public:
    modules_error_category() = default;

    virtual const char* name() const noexcept
    { 
        return "module name";
    }

    virtual std::string message(int c) const
    {
        switch (static_cast<ModuleErrorCode>(c)) {
            case ModuleErrorCode::Success:  return "OK";
            case ModuleErrorCode::Failed:   return "Failed";
            default:                        return "Unknown error";
        }
    }

    static const std::error_category& get()
    {
        const static class modules_error_category instance;
        return instance;
    }
};

std::error_code make_error_code(ModuleErrorCode ec)
{
    return std::error_code(static_cast<int>(ec), modules_error_category::get());
}

//case 3
std::error_code case3(void)
{
    return ModuleErrorCode::Failed;
}

//case 4
std::error_code case4(void)
{
    throw std::error_code(ModuleErrorCode::Success);
}

int main()
{
    //case 1
    std::error_code code1 = ModuleErrorCode::Success;
    std::cout << "code message(): " << code1.message() << std::endl;

    //case 2
    std::error_code code2(ModuleErrorCode::Failed);
    std::cout << "code message(): " << code2.message() << std::endl;

    //case 3
    std::error_code code3 = case3();
    std::cout << "code message(): " << code3.message() << std::endl;

    //case 4
    try {
        case4();
    } catch (std::error_code & e) {
        std::cout << "code message(): " << e.message() << std::endl;
    }

    std::cout << "code1 code(): " << code1.value() << std::endl;
    std::cout << "code1 name(): " << code1.category().name() << std::endl;
    std::cout << "code1 name(): " << code1.what() << std::endl;

    return 0;
}
