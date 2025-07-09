#pragma once

#include <iostream>
#include <gtest/gtest.h>

enum class Error
{
    NoError,
    CorruptedArchive
};

class Serializer
{
    static constexpr char Separator = ' ';
    
public:
    explicit Serializer(std::ostream& out) : out(&out) {}

    template <class T>
    Error save(T& object)
    {
        return object.serialize(*this);
    }

    template <class... ArgsT>
    Error operator()(ArgsT... args)
    {
        return iterate(args...);
    }

    Error iterate() { return Error::NoError; }
    Error process() { return Error::NoError; }

    template <class T, class... ArgsT>
    Error iterate(T&& first, ArgsT... args)
    {
        Error result = process(std::move(first));

        if (result == Error::NoError)
        {
            result = iterate(args...);
        }
        else
        {
            return Error::CorruptedArchive;
        }

        return result;
    }

    Error process(bool&& arg)
    {
        if (arg)
        {
            *out << "true" << Serializer::Separator;
        }
        else
        {
            *out << "false" << Serializer::Separator;
        }

        return Error::NoError;
    }

    Error process(uint64_t&& arg)
    {
        *out << arg << Serializer::Separator;

        return Error::NoError;
    }

private:
    std::ostream* out;
};


class Deserializer
{
    static constexpr char Separator = ' ';
    
public:
    explicit Deserializer(std::istream& in) : in(&in) {}

    template <class T>
    Error load(T& object)
    {
        return object.deserialize(*this);
    }

    template <class... ArgsT>
    Error operator()(ArgsT&... args)
    {
        return iterate(args...);
    }

    Error iterate() { return Error::NoError; }
    Error process() { return Error::NoError; }

    template <class T, class... ArgsT>
    Error iterate(T& first, ArgsT&... args)
    {
        Error result = process(first);

        if (result == Error::NoError)
        {
            result = iterate(args...);
        }
        else
        {
            return Error::CorruptedArchive;
        }

        return result;
    }

    Error process(bool& arg)
    {
        std::string text;
        *in >> text;

        if (text == "true")
        {
            arg = true;
        }
        else if (text == "false")
        {
            arg = false;
        }
        else
        {
            return Error::CorruptedArchive;
        }

        return Error::NoError;
    }

    Error process(uint64_t& arg)
    {
        std::string text;
        *in >> text;

        try
        {
            arg = std::stoull(text);
        }
        catch(const std::exception& e)
        {
            return Error::CorruptedArchive;
        }

        return Error::NoError;
    }

private:
    std::istream* in;
};
