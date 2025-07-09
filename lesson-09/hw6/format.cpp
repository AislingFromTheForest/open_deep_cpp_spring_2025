#include <sstream>
#include <cassert>
#include <iostream>
#include <unordered_map>
#include <concepts>
#include <gtest/gtest.h>

template<typename T>
concept Printable = requires(T x) {
    { std::cout << x } -> std::same_as<std::ostream&>;
};

class MYexception: public std::exception
{
public:
    MYexception(const std::string& message) : message(message)
    {}

    const char* what() const noexcept override
    {
        return message.c_str();
    }

private:
    std::string message;
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
            throw MYexception("INCORRECT INPUT");
        }

        if (l == std::string::npos && r == std::string::npos)
        {
            break;
        }

        if (l > r)
        {
            throw MYexception("INCORRECT INPUT");
        }

        std::string str2uint = input.substr(l + 1, r - l - 1);
        uint64_t _uint;

        if (!is_str_digit(str2uint))
        {
            throw MYexception("INCORRECT INPUT");
        }

        _uint = std::stoull(str2uint);
         
        if (_uint >= count)
        {
            
            throw MYexception("INCORRECT INPUT");
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

TEST(TEST_INPUT, 1)
{
    std::string input = "{{1}+{1} = {0}";
    try
    {
        format(input, 2, "one");
    }
    catch(const MYexception& e)
    {
        ASSERT_STREQ(e.what(), "INCORRECT INPUT");
    }
}

TEST(TEST_INPUT, 2)
{
    std::string input = "{1}}+{1} = {0}";
    try
    {
        format(input, 2, "one");
    }
    catch(const MYexception& e)
    {
        ASSERT_STREQ(e.what(), "INCORRECT INPUT");
    }
}

TEST(TEST_INPUT, 3)
{
    std::string input = "{1a}+{1} = {0}";
    try
    {
        format(input, 2, "one");
    }
    catch(const MYexception& e)
    {
        ASSERT_STREQ(e.what(), "INCORRECT INPUT");
    }
}

TEST(TEST_INPUT, 4)
{
    std::string input = "{2}+{1} = {0}";
    try
    {
        format(input, 2, "one");
    }
    catch(const MYexception& e)
    {
        ASSERT_STREQ(e.what(), "INCORRECT INPUT");
    }
}

TEST(TEST_INPUT, 5)
{
    std::string input = "{0}+{1} = {}";
    try
    {
        format(input, 2, "one");
    }
    catch(const MYexception& e)
    {
        ASSERT_STREQ(e.what(), "INCORRECT INPUT");
    }
}

TEST(TEST_CORRECTNESS, 1)
{
    std::string input = "{1}+{1} = {0}";
    std::string answer = format(input, 2, "one");
    ASSERT_STREQ("one+one = 2", answer.c_str());
}

TEST(TEST_CORRECTNESS, 2)
{
    std::string input = "{0}+{1}+{2} = {3}";
    std::string answer = format(input, "one", 2, 'c', true);
    ASSERT_STREQ("one+2+c = 1", answer.c_str());
}

TEST(TEST_CORRECTNESS, 3)
{
    std::string input = "{0}+{1}+{2}+{3} = {4}";
    std::string answer = format(input, "one", 2, 'c', "{haha}", true);
    ASSERT_STREQ("one+2+c+{haha} = 1", answer.c_str());
}

TEST(TEST_CORRECTNESS, 4)
{
    std::string input = "{0}+{1}+{2}+{3}+{3}+{3} = {4}";
    std::string answer = format(input, "one", 2, 'c', "{haha}", true);
    ASSERT_STREQ("one+2+c+{haha}+{haha}+{haha} = 1", answer.c_str());
}

int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
