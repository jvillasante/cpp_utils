#include <libutils/unique_handler.hpp>
#include <libutils/unused.hpp>

#include <catch2/catch_all.hpp>
#include <cstdio>
#include <fstream>
#include <iostream>
#include <memory>
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

std::string read(FILE* f)
{
    utils::unused(::fseek(f, 0, SEEK_END));
    auto position = ::ftell(f);
    if (position < 0)
    {
        std::cerr << "ftell failed" << '\n';
        return {};
    }
    ::rewind(f); // NOLINT
    auto file_size = static_cast<std::size_t>(position);

    std::string content;
    content.resize(file_size);
    utils::unused(fread(content.data(), sizeof(char), file_size, f));
    return content;
}
} // namespace

TEST_CASE("UniqueHandler - NotExistent")
{
    utils::UniqueHandle<CFileHandleTraits> handle(
        std::fopen("NonExistentFile", "r"));
    REQUIRE_FALSE(handle);
}

// NOLINTNEXTLINE
class UniqueHandleFixture
{
protected:
    std::string const file = "my_file.txt";
    std::string const file_contents = R"(line1<br />line2<br />line3<br />)";

public:
    UniqueHandleFixture()
    {
        std::ofstream fs(file);
        if (!fs)
        {
            std::cerr << "Cannot open the output file " << file << '\n';
            FAIL("Cannot open the output file");
        }
        fs << file_contents;
        fs.close();
    };
    ~UniqueHandleFixture() { utils::unused(std::remove(file.c_str())); }
};

TEST_CASE_METHOD(UniqueHandleFixture, "ShouldWork")
{
    utils::UniqueHandle<CFileHandleTraits> handle{
        std::fopen(file.c_str(), "r")};
    REQUIRE(handle);

    std::string content{read(*handle)};
    REQUIRE(content == file_contents);

    utils::UniqueHandle<CFileHandleTraits> handle1{std::move(handle)};
    REQUIRE_FALSE(handle);
    REQUIRE(handle1);
    content = read(*handle1);
    REQUIRE(content == file_contents);
}

namespace
{
struct FileDeleter
{
    void operator()(FILE* f) { utils::unused(std::fclose(f)); }
};
} // namespace

TEST_CASE_METHOD(UniqueHandleFixture, "UniquePtr")
{
    using unique_file_t = std::unique_ptr<FILE, FileDeleter>;
    unique_file_t handle{std::fopen(file.c_str(), "r")};

    REQUIRE(handle);

    std::string content{read(handle.get())};
    REQUIRE(content == file_contents);

    unique_file_t handle1{std::move(handle)};
    REQUIRE_FALSE(handle);
    REQUIRE(handle1);
    content = read(handle1.get());
    REQUIRE(content == file_contents);

    unique_file_t handle2;
    handle2 = std::move(handle1);
    REQUIRE_FALSE(handle1);
    REQUIRE(handle2);
    content = read(handle2.get());
    REQUIRE(content == file_contents);
}

namespace
{
struct IntHandleTraits
{
    using handle = int;
    static handle invalid() noexcept { return -1; }
    static void destroy(handle value) noexcept
    {
        if (value != IntHandleTraits::invalid())
        {
            value = IntHandleTraits::invalid();
        }
    }
};
} // namespace

TEST_CASE_METHOD(UniqueHandleFixture, "IntHandle")
{
    utils::UniqueHandle<IntHandleTraits> handle(42);
    REQUIRE(handle);
    REQUIRE(handle.get() == 42);

    utils::UniqueHandle<IntHandleTraits> handle1{std::move(handle)};
    REQUIRE_FALSE(handle);
    REQUIRE(handle1);
    REQUIRE(handle.get() == -1);
    REQUIRE(handle1.get() == 42);

    utils::UniqueHandle<IntHandleTraits> handle2;
    handle2 = std::move(handle1);
    REQUIRE_FALSE(handle1);
    REQUIRE(handle2);
    REQUIRE(handle1.get() == -1);
    REQUIRE(handle2.get() == 42);

    handle2.reset();
    REQUIRE_FALSE(handle2);
    REQUIRE(handle2.get() == -1);
}
