#include "serializer.h"
#include <sstream>
#include <cassert>

struct Data
{
    uint64_t a;
    bool     b;
    uint64_t c;

    template <class Serializer>
    Error serialize(Serializer& serializer)
    {
        return serializer(std::move(a), std::move(b), std::move(c));
    }

    template <class Deserializer>
    Error deserialize(Deserializer& deserializer)
    {
        return deserializer(a, b, c);
    }
};

struct Data2
{
    uint64_t a;
    bool     b;
    uint64_t c;
    bool     d;

    template <class Serializer>
    Error serialize(Serializer& serializer)
    {
        return serializer(std::move(a), std::move(b), std::move(c), std::move(d));
    }

    template <class Deserializer>
    Error deserialize(Deserializer& deserializer)
    {
        return deserializer(a, b, c, d);
    }
};

int main()
{
    std::stringstream stream;
    Serializer serializer(stream);
    Deserializer deserializer(stream);
    Error err;


    // DATA
    {
        Data x {3, true, 4};
        Data y {0, false, 0};

        serializer.save(x);
        err = deserializer.load(y);

        assert(err == Error::NoError);
        assert(x.a == y.a);
        assert(x.b == y.b);
        assert(x.c == y.c);
    }

    {
        Data x {500, false, 1000};
        Data y {0, false, 0};

        serializer.save(x);
        err = deserializer.load(y);

        assert(err == Error::NoError);
        assert(x.a == y.a);
        assert(x.b == y.b);
        assert(x.c == y.c);
    }

    {
        Data x  {500, false, 1000};
        Data xx {5000, true, 10000};
        Data y  {0, false, 0};
        Data yy {0, false, 0};

        serializer.save(x);
        serializer.save(xx);

        err = deserializer.load(y);
        assert(err == Error::NoError);
        assert(x.a == y.a);
        assert(x.b == y.b);
        assert(x.c == y.c);

        err = deserializer.load(yy);
        assert(err == Error::NoError);
        assert(xx.a == yy.a);
        assert(xx.b == yy.b);
        assert(xx.c == yy.c);
    }

    {
        Data x  {500, false, 1000};
        Data xx {5000, true, 10000};
        Data y  {0, false, 0};
        Data yy {0, false, 0};

        serializer.save(x);
        stream << "false msg";
        serializer.save(xx);

        err = deserializer.load(y);
        assert(err == Error::NoError);
        assert(x.a == y.a);
        assert(x.b == y.b);
        assert(x.c == y.c);

        err = deserializer.load(yy);
        assert(err == Error::CorruptedArchive);
    }

    stream = std::stringstream();

    // DATA2
    {
        Data2 x {3, true, 4, false};
        Data2 y {0, false, 0, false};

        serializer.save(x);
        err = deserializer.load(y);

        assert(err == Error::NoError);
        assert(x.a == y.a);
        assert(x.b == y.b);
        assert(x.c == y.c);
        assert(x.d == y.d);
    }

    {
        Data2 x {500, false, 1000, true};
        Data2 y {0, false, 0, false};

        serializer.save(x);
        err = deserializer.load(y);

        assert(err == Error::NoError);
        assert(x.a == y.a);
        assert(x.b == y.b);
        assert(x.c == y.c);
        assert(x.d == y.d);
    }

    {
        Data2 x  {500, false, 1000, true};
        Data2 xx {5000, true, 10000, false};
        Data2 y  {0, false, 0};
        Data2 yy {0, false, 0};

        serializer.save(x);
        serializer.save(xx);

        err = deserializer.load(y);
        assert(err == Error::NoError);
        assert(x.a == y.a);
        assert(x.b == y.b);
        assert(x.c == y.c);
        assert(x.d == y.d);

        err = deserializer.load(yy);
        assert(err == Error::NoError);
        assert(xx.a == yy.a);
        assert(xx.b == yy.b);
        assert(xx.d == yy.d);
    }

    std::cout << "TESTS PASSED" << std::endl;

    return 0;
}