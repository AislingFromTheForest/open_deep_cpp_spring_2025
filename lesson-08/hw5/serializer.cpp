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


TEST(EqualityTest, TestCorrectnessData_1) {
    std::stringstream stream;
    Serializer serializer(stream);
    Deserializer deserializer(stream);
    Error err;

    Data x {3, true, 4};
    Data y {0, false, 0};

    serializer.save(x);
    err = deserializer.load(y);

    ASSERT_EQ(err, Error::NoError);
    ASSERT_EQ(x.a, y.a);
    ASSERT_EQ(x.b, y.b);
    ASSERT_EQ(x.c, y.c);
}


TEST(EqualityTest, TestCorrectnessData_2) {
    std::stringstream stream;
    Serializer serializer(stream);
    Deserializer deserializer(stream);
    Error err;

    Data x {500, false, 1000};
    Data y {0, false, 0};

    serializer.save(x);
    err = deserializer.load(y);

    ASSERT_EQ(err, Error::NoError);
    ASSERT_EQ(x.a, y.a);
    ASSERT_EQ(x.b, y.b);
    ASSERT_EQ(x.c, y.c);
}

TEST(EqualityTest, TestCorrectnessData_3) {
    std::stringstream stream;
    Serializer serializer(stream);
    Deserializer deserializer(stream);
    Error err;

    Data x  {500, false, 1000};
    Data xx {5000, true, 10000};
    Data y  {0, false, 0};
    Data yy {0, false, 0};

    serializer.save(x);
    serializer.save(xx);

    err = deserializer.load(y);
    ASSERT_EQ(err, Error::NoError);
    ASSERT_EQ(x.a, y.a);
    ASSERT_EQ(x.b, y.b);
    ASSERT_EQ(x.c, y.c);

    err = deserializer.load(yy);
    ASSERT_EQ(err, Error::NoError);
    ASSERT_EQ(xx.a, yy.a);
    ASSERT_EQ(xx.b, yy.b);
    ASSERT_EQ(xx.c, yy.c);
}

TEST(EqualityTest, TestIncorrectnessData) {
    std::stringstream stream;
    Serializer serializer(stream);
    Deserializer deserializer(stream);
    Error err;

    Data x  {500, false, 1000};
    Data xx {5000, true, 10000};
    Data y  {0, false, 0};
    Data yy {0, false, 0};

    serializer.save(x);
    stream << "false msg";
    serializer.save(xx);

    err = deserializer.load(y);
    ASSERT_EQ(err, Error::NoError);
    ASSERT_EQ(x.a, y.a);
    ASSERT_EQ(x.b, y.b);
    ASSERT_EQ(x.c, y.c);

    err = deserializer.load(yy);
    ASSERT_EQ(err, Error::CorruptedArchive);
}


TEST(EqualityTest, TestCorrectnessData_4) {
    std::stringstream stream;
    Serializer serializer(stream);
    Deserializer deserializer(stream);
    Error err;

    Data2 x {3, true, 4, false};
    Data2 y {0, false, 0, false};

    serializer.save(x);
    err = deserializer.load(y);

    ASSERT_EQ(err, Error::NoError);
    ASSERT_EQ(x.a, y.a);
    ASSERT_EQ(x.b, y.b);
    ASSERT_EQ(x.c, y.c);
    ASSERT_EQ(x.d, y.d);
}


TEST(EqualityTest, TestCorrectnessData_5) {
    std::stringstream stream;
    Serializer serializer(stream);
    Deserializer deserializer(stream);
    Error err;

    Data2 x {500, false, 1000, true};
    Data2 y {0, false, 0, false};

    serializer.save(x);
    err = deserializer.load(y);

    ASSERT_EQ(err, Error::NoError);
    ASSERT_EQ(x.a, y.a);
    ASSERT_EQ(x.b, y.b);
    ASSERT_EQ(x.c, y.c);
    ASSERT_EQ(x.d, y.d);
}

TEST(EqualityTest, TestCorrectnessData_6) {
    std::stringstream stream;
    Serializer serializer(stream);
    Deserializer deserializer(stream);
    Error err;

    Data2 x  {500, false, 1000, true};
    Data2 xx {5000, true, 10000, false};
    Data2 y  {0, false, 0};
    Data2 yy {0, false, 0};

    serializer.save(x);
    serializer.save(xx);

    err = deserializer.load(y);
    ASSERT_EQ(err, Error::NoError);
    ASSERT_EQ(x.a, y.a);
    ASSERT_EQ(x.b, y.b);
    ASSERT_EQ(x.c, y.c);
    ASSERT_EQ(x.d, y.d);

    err = deserializer.load(yy);
    ASSERT_EQ(err, Error::NoError);
    ASSERT_EQ(xx.a, yy.a);
    ASSERT_EQ(xx.b, yy.b);
    ASSERT_EQ(xx.c, yy.c);
    ASSERT_EQ(xx.d, yy.d);
}

int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}