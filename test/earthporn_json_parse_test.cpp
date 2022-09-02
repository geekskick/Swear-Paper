#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "earthporn.hpp"
#include "expected_reply.hpp"

namespace {
struct mock_delegate : public json_parse_delegate_b {
  mock_delegate() : json_parse_delegate_b{nullptr} {}
  MOCK_METHOD(void, parse_result, (const std::string &), (const, override));
};
}  // namespace

TEST(JsonParseTest, EarthPornURL) {
  constexpr auto expected = "https://www.reddit.com/r/earthporn/new.json?sort=new";
  const auto uut = earthporn{nullptr};
  const auto actual = uut.get_sub_reddit_url();
  ASSERT_EQ(actual, expected);
}
TEST(JsonParseTest, CanParseJsonWithoutSkipping) {
  const auto uut = earthporn{nullptr};
  const auto actual = uut.get_url_from_reply(expected_reply, 0);
  const auto expected = "https://i.imgur.com/vHN43wI.jpg";
  ASSERT_EQ(actual, expected);
}

TEST(JsonParseTest, CanParseJsonWithSkipping) {
  const auto uut = earthporn{nullptr};
  const auto actual = uut.get_url_from_reply(expected_reply, 1);
  const auto expected = "https://i.redd.it/vy6ag3o23fl91.jpg";
  ASSERT_EQ(actual, expected);
}

TEST(JsonParseTest, CanParseJsonWithSkippingOOB) {
  const auto uut = earthporn{nullptr};
  ASSERT_ANY_THROW(uut.get_url_from_reply(expected_reply, 2));
}

TEST(JsonParseTest, CantParseInvalid) {
  const auto uut = earthporn{nullptr};
  ASSERT_ANY_THROW(uut.get_url_from_reply("expected_reply", 0));
}

TEST(JsonParseTest, CallsDelegateWhenSuccessful) {
  auto mock = std::make_unique<mock_delegate>();
  EXPECT_CALL(*mock, parse_result(testing::_));

  const auto uut = earthporn{std::move(mock)};
  const auto actual = uut.get_url_from_reply(expected_reply, 0);
  const auto expected = "https://i.imgur.com/vHN43wI.jpg";
  ASSERT_EQ(actual, expected);
}

TEST(JsonParseTest, DoesntCallsDelegateWhenUnsuccessful) {
  auto mock = std::make_unique<testing::StrictMock<mock_delegate>>();

  const auto uut = earthporn{std::move(mock)};
  ASSERT_ANY_THROW(uut.get_url_from_reply(expected_reply, 2));
}