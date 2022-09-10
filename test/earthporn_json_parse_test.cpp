#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "earthporn.hpp"
#include "expected_reply.hpp"

TEST(JsonParseTest, EarthPornURL) {
  constexpr auto expected = "https://www.reddit.com/r/earthporn/new.json?sort=new";
  const auto uut = earthporn{};
  const auto actual = uut.get_sub_reddit_url();
  ASSERT_EQ(actual, expected);
}
TEST(JsonParseTest, CanParseJsonWithoutSkipping) {
  const auto uut = earthporn{};
  const auto actual = uut.get_url_from_reply(expected_reply, 0);
  const auto expected = "https://i.imgur.com/vHN43wI.jpg";
  ASSERT_EQ(actual, expected);
}

TEST(JsonParseTest, CanParseJsonWithSkipping) {
  const auto uut = earthporn{};
  const auto actual = uut.get_url_from_reply(expected_reply, 1);
  const auto expected = "https://i.redd.it/vy6ag3o23fl91.jpg";
  ASSERT_EQ(actual, expected);
}

TEST(JsonParseTest, CanParseJsonWithSkippingOOB) {
  const auto uut = earthporn{};
  ASSERT_ANY_THROW(uut.get_url_from_reply(expected_reply, 2));
}

TEST(JsonParseTest, CantParseInvalid) {
  const auto uut = earthporn{};
  ASSERT_ANY_THROW(uut.get_url_from_reply("expected_reply", 0));
}
