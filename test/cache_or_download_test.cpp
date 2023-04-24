#include "cache_or_download.hpp"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <fstream>

namespace {
struct MockDownloader : public downloader_interface {
  MOCK_METHOD(std::optional<std::string>, perform_string, (const std::string &), (const, override));
  MOCK_METHOD(std::optional<std::vector<std::string>>, perform_vector, (const std::string &), (const, override));
  MOCK_METHOD(std::optional<std::vector<char>>, perform_image, (const std::string &), (const, override));
};

std::string ReadFile(const std::filesystem::path &file) {
  auto f = std::ifstream{file};
  auto ss = std::stringstream{};
  ss << f.rdbuf();
  return ss.str();
}

struct TempFile {
  std::filesystem::path path{};
  bool parent_existed{false};

  TempFile(std::filesystem::path p, std::string c) : path{std::move(p)} {
    const auto parent = path.parent_path();
    parent_existed = std::filesystem::exists(parent);
    if (!parent_existed) {
      std::cout << "Creating " << parent.c_str() << "\n";
      const auto parent_made = std::filesystem::create_directories(parent);
      if (!parent_made) {
        throw;
      }
    }

    std::ofstream f{path};
    f << c;
  }

  ~TempFile() {
    std::filesystem::remove(path);
    if (!parent_existed) {
      std::cout << "Removing " << path.parent_path() << "\n";
      std::filesystem::remove(path.parent_path());
    }
  }
};

}  // namespace

TEST(CacheOrDownloadTest, GetFromCache) {
  const auto file = TempFile{std::filesystem::temp_directory_path() / "GetFromCache.txt", "help"};
  const auto url = "RUBBISH";
  const auto downloader = ::testing::StrictMock<MockDownloader>{};
  auto uut = CacheOrDownloadStrings{file.path, url, downloader};
  const auto actual = uut.get();
  ASSERT_EQ(actual.value(), std::vector<std::string>{"help"});
  ASSERT_TRUE(uut);
}

TEST(CacheOrDownloadTest, GetFromCacheManyWords) {
  const auto file = TempFile{std::filesystem::temp_directory_path() / "GetFromCacheManyWords.txt", "help me"};
  const auto url = "RUBBISH";
  const auto downloader = ::testing::StrictMock<MockDownloader>{};
  auto uut = CacheOrDownloadStrings{file.path, url, downloader};
  const auto actual = uut.get();
  ASSERT_EQ(actual.value(), std::vector<std::string>{"help me"});
  ASSERT_TRUE(uut);
}

TEST(CacheOrDownloadTest, GetFromCacheManyLines) {
  const auto file = TempFile{std::filesystem::temp_directory_path() / "GetFromCacheManyLines.txt", "help me\nholy"};
  const auto url = "RUBBISH";
  const auto downloader = ::testing::StrictMock<MockDownloader>{};
  auto uut = CacheOrDownloadStrings{file.path, url, downloader};
  const auto actual = uut.get();
  const auto expected = std::vector<std::string>{"help me", "holy"};
  ASSERT_EQ(actual.value(), expected);
  ASSERT_TRUE(uut);
}

TEST(CacheOrDownloadTest, GetFromUrl) {
  const auto file = std::filesystem::temp_directory_path() / "THISPROBABLYDOESNTEXIST";
  std::filesystem::remove(file);
  EXPECT_FALSE(std::filesystem::exists(file));
  const auto url = "RUBBISH";
  const auto downloader = ::testing::StrictMock<MockDownloader>{};
  EXPECT_CALL(downloader, perform_vector(url)).WillOnce(::testing::Return(std::vector<std::string>{"one", "two"}));
  auto uut = CacheOrDownloadStrings{file, url, downloader};
  const auto actual = uut.get();
  ASSERT_EQ(actual.value(), (std::vector<std::string>{"one", "two"}));
  ASSERT_TRUE(uut);

  // Is written to the cache file
  ASSERT_TRUE(std::filesystem::exists(file));
  const auto from_file = ReadFile(file);
  ASSERT_EQ(from_file, "one\ntwo\n");

  // Doesn't reply with anything else
  // This is caught with the mock, we know it's not going to download anything else
  ASSERT_EQ(actual, uut.get());
}

TEST(CacheOrDownloadTest, GetFromUrlOptionalOnFail) {
  const auto file = std::filesystem::temp_directory_path() / "THISPROBABLYDOESNTEXIST2";
  std::filesystem::remove(file);
  EXPECT_FALSE(std::filesystem::exists(file));
  const auto url = "RUBBISH";
  const auto downloader = ::testing::StrictMock<MockDownloader>{};
  EXPECT_CALL(downloader, perform_vector(url)).WillOnce(::testing::Throw(std::runtime_error("foo")));
  auto uut = CacheOrDownloadStrings{file, url, downloader};
  const auto actual = uut.get();
  ASSERT_FALSE(actual.has_value());
  ASSERT_FALSE(uut);
}