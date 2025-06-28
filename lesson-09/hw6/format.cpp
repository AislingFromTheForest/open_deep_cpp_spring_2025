#include <sstream>
#include <cassert>
#include <iostream>
#include <unordered_map>
#include <concepts>

template<typename T>
concept Printable = requires(T x) {
    { std::cout << x } -> std::same_as<std::ostream&>;
};

class Serializer
{
    static constexpr char Separator = ' ';
    
public:
    explicit Serializer(std::ostream& out) : out(&out) {}


    template <Printable... ArgsT>
    void operator()(ArgsT... args)
    {
        return iterate(args...);
    }

    void iterate() { return; }
    void process() { return; }

    template <class T, Printable... ArgsT>
    void iterate(T&& first, ArgsT... args)
    {
        process(std::move(first));
        iterate(args...);

        return;
    }

    template <class T>
    void process(T&& arg)
    {
        *out << arg << Serializer::Separator;
    }

private:
    std::ostream* out;
};

template <Printable... ArgsT>
std::unordered_map<size_t, std::string> args2map(ArgsT&... args)
{
    std::stringstream ss;

    Serializer serializer(ss);
    serializer(args...);

    size_t cnt = 0;
    std::string str;
    std::unordered_map<size_t, std::string> id2str;

    while (ss >> str)
    {
        id2str[cnt++] = str;
    }

    return id2str;
}

bool is_str_digit(const std::string& input)
{
    if (input.size() == 0)
    {
        return false;
    }

    for (char c : input)
    {
        if (!isdigit(c))
        {
            return false;
        }
    }

    return true;
}

void validation(const std::string& input, uint64_t count)
{
    size_t l, r, pos = 0;

    while (true)
    {
        l = input.find('{', pos);
        r = input.find('}', pos);

        if ((l != std::string::npos && r == std::string::npos) || 
            (l == std::string::npos && r != std::string::npos))
        {
            throw std::runtime_error("INCORRECT INPUT");
        }

        if (l == std::string::npos && r == std::string::npos)
        {
            break;
        }

        if (l > r)
        {
            throw std::runtime_error("INCORRECT INPUT");
        }

        std::string str2uint = input.substr(l + 1, r - l - 1);
        uint64_t _uint;

        if (!is_str_digit(str2uint))
        {
            throw std::runtime_error("INCORRECT INPUT");
        }

        _uint = std::stoull(str2uint);
         
        if (_uint >= count)
        {
            
            throw std::runtime_error("INCORRECT INPUT");
        }

        pos = r + 1;
    }
}


template <Printable... ArgsT>
std::string format(const std::string& input, ArgsT... args)
{
    validation(input, sizeof...(args));

    std::string result(input);
    std::unordered_map<size_t, std::string> id2str(args2map(args...));

    size_t l, r, pos = 0;

    while (true)
    {
        l = result.find('{', pos);
        r = result.find('}', pos);

        if (l == std::string::npos && r == std::string::npos)
        {
            break;
        }

        std::string str2uint = result.substr(l + 1, r - l - 1);
        uint64_t idx = std::stoull(str2uint);

        result.replace(l, r - l + 1, id2str[idx]);

        pos = l + id2str[idx].size();
    }

    return result;
}

void TEST_INPUT()
{

    std::string input;
    std::string text;

    try
    {
        input = "{{1}+{1} = {0}";
        text = format(input, 2, "one");
    }
    catch(const std::exception& e)
    {
        assert(std::string(e.what()) == "INCORRECT INPUT");
    }

    try
    {
        input = "{1}}+{1} = {0}";
        text = format(input, 2, "one");
    }
    catch(const std::exception& e)
    {
        assert(std::string(e.what()) == "INCORRECT INPUT");
    }

    try
    {
        input = "{1a}+{1} = {0}";
        text = format(input, 2, "one");
    }
    catch(const std::exception& e)
    {
        assert(std::string(e.what()) == "INCORRECT INPUT");
    }

    try
    {
        input = "{2}+{1} = {0}";
        text = format(input, 2, "one");
    }
    catch(const std::exception& e)
    {
        assert(std::string(e.what()) == "INCORRECT INPUT");
    }

    try
    {
        input = "{0}+{1} = {}";
        text = format(input, 2, "one");
    }
    catch(const std::exception& e)
    {
        assert(std::string(e.what()) == "INCORRECT INPUT");
    }
}

void TEST_CORRECTNESS()
{
    std::string input;
    std::string text;

    input = "{1}+{1} = {0}";
    text  = format(input, 2, "one");
    assert(text == "one+one = 2");

    input = "{0}+{1}+{2} = {3}";
    text  = format(input, "one", 2, 'c', true);
    assert(text == "one+2+c = 1");

    input = "{0}+{1}+{2}+{3} = {4}";
    text  = format(input, "one", 2, 'c', "{haha}", true);
    assert(text == "one+2+c+{haha} = 1");

    input = "{0}+{1}+{2}+{3}+{3}+{3} = {4}";
    text  = format(input, "one", 2, 'c', "{haha}", true);
    assert(text == "one+2+c+{haha}+{haha}+{haha} = 1");
}

int main()
{
    TEST_INPUT();
    TEST_CORRECTNESS();

    std::cout << "TESTS PASSED" << std::endl;

    return 0;
}
