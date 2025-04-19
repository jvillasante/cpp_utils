#include "unique_handler.hpp"
#include "unused.hpp"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <cstdio>
#include <fstream>
#include <string>
#include <utility>

namespace
{
struct CFileHandleTraits
{
    using handle = FILE*;
    static handle invalid() noexcept { return nullptr; }
    static void destroy(handle value) noexcept
    {
        if (value != CFileHandleTraits::invalid())
        {
            utils::unused(std::fclose(value));
        }
    }
};

std::string read(utils::UniqueHandle<CFileHandleTraits> const& handle)
{
    utils::unused(::fseek(*handle, 0, SEEK_END));
    auto position = ::ftell(*handle);
    if (position < 0)
    {
        std::cerr << "ftell failed" << '\n';
        return {};
    }
    ::rewind(*handle); // NOLINT
    auto file_size = static_cast<std::size_t>(position);

    std::string content;
    content.resize(file_size);
    utils::unused(fread(content.data(), sizeof(char), file_size, *handle));
    return content;
}
} // namespace

TEST(UniqueHandle, NotExistent)
{
    utils::UniqueHandle<CFileHandleTraits> handle(
        std::fopen("NonExistentFile", "r"));
    EXPECT_FALSE(handle);
}

class UniqueHandleTest : public ::testing::Test
{
protected:
    std::string const file = "my_file.txt";
    std::string const file_contents = R"(line1<br />line2<br />line3<br />)";
    void SetUp() override
    {
        std::ofstream fs(file);
        if (!fs)
        {
            std::cerr << "Cannot open the output file " << file << '\n';
            FAIL();
        }
        fs << file_contents;
        fs.close();
    }
    void TearDown() override { utils::unused(std::remove(file.c_str())); }
};

TEST_F(UniqueHandleTest, ShouldWork)
{
    utils::UniqueHandle<CFileHandleTraits> handle{
        std::fopen(file.c_str(), "r")};
    EXPECT_TRUE(handle);

    std::string content{read(handle)};
    EXPECT_EQ(content, file_contents);

    utils::UniqueHandle<CFileHandleTraits> handle1{std::move(handle)};
    EXPECT_FALSE(handle);
    EXPECT_TRUE(handle1);
    content = read(handle1);
    EXPECT_EQ(content, file_contents);

    utils::UniqueHandle<CFileHandleTraits> handle2;
    handle2 = std::move(handle1);
    EXPECT_FALSE(handle1);
    EXPECT_TRUE(handle2);
    content = read(handle2);
    EXPECT_EQ(content, file_contents);
}
