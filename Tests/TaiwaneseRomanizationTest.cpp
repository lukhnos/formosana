#include "gtest/gtest.h"

#include "TaiwaneseRomanization.h"
#include "VowelHelper.h"

namespace {

Formosa::TaiwaneseRomanization::RomanizationSyllable Compose(std::string input,
                                                             Formosa::TaiwaneseRomanization::SyllableType syllableType,
                                                             int tone,
                                                             bool usePOJLegacyOU = false) {
  Formosa::TaiwaneseRomanization::RomanizationSyllable syl;
  syl.setInputType(syllableType);
  syl.setUsePOJLegacyOU(usePOJLegacyOU);
  for (char &c : input) {
    if (c >= '0' && c <= '9') {
      syl.normalize(c - '0');
    } else {
      syl.insertCharacterAtCursor(c);
    }
  }
  return syl;

}

std::string ComposePOJ(std::string input, int tone = 0) {
  return Compose(input,
                 Formosa::TaiwaneseRomanization::POJSyllable,
                 tone).composedForm();
}

std::string ComposeLegacyPOJ(std::string input, int tone = 0) {
  return Compose(input,
                 Formosa::TaiwaneseRomanization::POJSyllable,
                 tone,
                 true).composedForm();
}

std::string ComposeTL(std::string input, int tone = 0) {
  return Compose(input,
                 Formosa::TaiwaneseRomanization::TLSyllable,
                 tone).composedForm();
}

std::string ComposeTLWithPOJStyleNN(std::string input, int tone = 0) {
  Formosa::TaiwaneseRomanization::RomanizationSyllable
      syl = Compose(input, Formosa::TaiwaneseRomanization::TLSyllable, tone);
  syl.setForcePOJStyle(true);
  return syl.composedForm();
}

std::string ComposeLegacyTLWithPOJStyleNN(std::string input, int tone = 0) {
  Formosa::TaiwaneseRomanization::RomanizationSyllable
      syl =
      Compose(input, Formosa::TaiwaneseRomanization::TLSyllable, tone, true);
  syl.setForcePOJStyle(true);
  return syl.composedForm();
}

TEST(TaiwaneseRomanizationTest, BasicTest) {
  EXPECT_EQ(1, 1);
}

TEST(TaiwaneseRomanizationTest, OuTestPOJ) {
  EXPECT_EQ(ComposePOJ("Ou"), "O\u0358");
  EXPECT_EQ(ComposePOJ("Ou2"), "O\u0358\u0301");
  EXPECT_EQ(ComposePOJ("Ou3"), "O\u0358\u0300");
  EXPECT_EQ(ComposePOJ("Ou5"), "O\u0358\u0302");
  EXPECT_EQ(ComposePOJ("Ou6"), "O\u0358\u030c");
  EXPECT_EQ(ComposePOJ("Ou7"), "O\u0358\u0304");
  EXPECT_EQ(ComposePOJ("Ouh8"), "O\u0358\u030dh");
  EXPECT_EQ(ComposePOJ("Ou9"), "O\u0358\u0306");

  EXPECT_EQ(ComposePOJ("ou"), "o\u0358");
  EXPECT_EQ(ComposePOJ("ou2"), "o\u0358\u0301");
  EXPECT_EQ(ComposePOJ("ou3"), "o\u0358\u0300");
  EXPECT_EQ(ComposePOJ("ou5"), "o\u0358\u0302");
  EXPECT_EQ(ComposePOJ("ou6"), "o\u0358\u030c");
  EXPECT_EQ(ComposePOJ("ou7"), "o\u0358\u0304");
  EXPECT_EQ(ComposePOJ("ouh8"), "o\u0358\u030dh");
  EXPECT_EQ(ComposePOJ("ou9"), "o\u0358\u0306");
}

TEST(TaiwaneseRomanizationTest, LegacyOuTestPOJ) {
  EXPECT_EQ(ComposeLegacyPOJ("Ou"), "O\u0358");
  EXPECT_EQ(ComposeLegacyPOJ("Ou2"), "Ó\u0358");
  EXPECT_EQ(ComposeLegacyPOJ("Ou3"), "Ò\u0358");
  EXPECT_EQ(ComposeLegacyPOJ("Ou5"), "Ô\u0358");
  EXPECT_EQ(ComposeLegacyPOJ("Ou6"), "Ǒ\u0358");
  EXPECT_EQ(ComposeLegacyPOJ("Ou7"), "Ō\u0358");
  EXPECT_EQ(ComposeLegacyPOJ("Ouh8"), "O\u030d\u0358h");
  EXPECT_EQ(ComposeLegacyPOJ("Ou9"), "Ŏ\u0358");

  EXPECT_EQ(ComposeLegacyPOJ("ou"), "o\u0358");
  EXPECT_EQ(ComposeLegacyPOJ("ou2"), "ó\u0358");
  EXPECT_EQ(ComposeLegacyPOJ("ou3"), "ò\u0358");
  EXPECT_EQ(ComposeLegacyPOJ("ou5"), "ô\u0358");
  EXPECT_EQ(ComposeLegacyPOJ("ou6"), "ǒ\u0358");
  EXPECT_EQ(ComposeLegacyPOJ("ou7"), "ō\u0358");
  EXPECT_EQ(ComposeLegacyPOJ("ouh8"), "o\u030d\u0358h");
  EXPECT_EQ(ComposeLegacyPOJ("ou9"), "ŏ\u0358");
}

TEST(TaiwaneseRomanizationTest, OoTestTL) {
  EXPECT_EQ(ComposeTL("Oo"), "Oo");
  EXPECT_EQ(ComposeTL("Oo2"), "Óo");
  EXPECT_EQ(ComposeTL("Oo3"), "Òo");
  EXPECT_EQ(ComposeTL("Oo5"), "Ôo");
  EXPECT_EQ(ComposeTL("Oo6"), "Ǒo");
  EXPECT_EQ(ComposeTL("Oo7"), "Ōo");
  EXPECT_EQ(ComposeTL("Ooh8"), "O\u030doh");
  EXPECT_EQ(ComposeTL("Oo9"), "Őo");

  EXPECT_EQ(ComposeTL("oo"), "oo");
  EXPECT_EQ(ComposeTL("oo2"), "óo");
  EXPECT_EQ(ComposeTL("oo3"), "òo");
  EXPECT_EQ(ComposeTL("oo5"), "ôo");
  EXPECT_EQ(ComposeTL("oo6"), "ǒo");
  EXPECT_EQ(ComposeTL("oo7"), "ōo");
  EXPECT_EQ(ComposeTL("ooh8"), "o\u030doh");
  EXPECT_EQ(ComposeTL("oo9"), "őo");
}

TEST(TaiwaneseRomanizationTest, OoTestTLWithPOJNN) {
  EXPECT_EQ(ComposeTLWithPOJStyleNN("Oo"), "O\u0358");
  EXPECT_EQ(ComposeTLWithPOJStyleNN("Oo2"), "O\u0358\u0301");
  EXPECT_EQ(ComposeTLWithPOJStyleNN("Oo3"), "O\u0358\u0300");
  EXPECT_EQ(ComposeTLWithPOJStyleNN("Oo5"), "O\u0358\u0302");
  EXPECT_EQ(ComposeTLWithPOJStyleNN("Oo6"), "O\u0358\u030c");
  EXPECT_EQ(ComposeTLWithPOJStyleNN("Oo7"), "O\u0358\u0304");
  EXPECT_EQ(ComposeTLWithPOJStyleNN("Ooh8"), "O\u0358\u030dh");
  EXPECT_EQ(ComposeTLWithPOJStyleNN("Oo9"), "O\u0358\u030b");

  EXPECT_EQ(ComposeTLWithPOJStyleNN("oo"), "o\u0358");
  EXPECT_EQ(ComposeTLWithPOJStyleNN("oo2"), "o\u0358\u0301");
  EXPECT_EQ(ComposeTLWithPOJStyleNN("oo3"), "o\u0358\u0300");
  EXPECT_EQ(ComposeTLWithPOJStyleNN("oo5"), "o\u0358\u0302");
  EXPECT_EQ(ComposeTLWithPOJStyleNN("oo6"), "o\u0358\u030c");
  EXPECT_EQ(ComposeTLWithPOJStyleNN("oo7"), "o\u0358\u0304");
  EXPECT_EQ(ComposeTLWithPOJStyleNN("ooh8"), "o\u0358\u030dh");
  EXPECT_EQ(ComposeTLWithPOJStyleNN("oo9"), "o\u0358\u030b");
}

TEST(TaiwaneseRomanizationTest, OoTestLegacyTLWithPOJNN) {
  EXPECT_EQ(ComposeLegacyTLWithPOJStyleNN("Oo"), "O\u0358");
  EXPECT_EQ(ComposeLegacyTLWithPOJStyleNN("Oo2"), "Ó\u0358");
  EXPECT_EQ(ComposeLegacyTLWithPOJStyleNN("Oo3"), "Ò\u0358");
  EXPECT_EQ(ComposeLegacyTLWithPOJStyleNN("Oo5"), "Ô\u0358");
  EXPECT_EQ(ComposeLegacyTLWithPOJStyleNN("Oo6"), "Ǒ\u0358");
  EXPECT_EQ(ComposeLegacyTLWithPOJStyleNN("Oo7"), "Ō\u0358");
  EXPECT_EQ(ComposeLegacyTLWithPOJStyleNN("Ooh8"), "O\u030d\u0358h");
  EXPECT_EQ(ComposeLegacyTLWithPOJStyleNN("Oo9"), "O\u030b\u0358");

  EXPECT_EQ(ComposeLegacyTLWithPOJStyleNN("oo"), "o\u0358");
  EXPECT_EQ(ComposeLegacyTLWithPOJStyleNN("oo2"), "ó\u0358");
  EXPECT_EQ(ComposeLegacyTLWithPOJStyleNN("oo3"), "ò\u0358");
  EXPECT_EQ(ComposeLegacyTLWithPOJStyleNN("oo3"), "ò\u0358");
  EXPECT_EQ(ComposeLegacyTLWithPOJStyleNN("oo5"), "ô\u0358");
  EXPECT_EQ(ComposeLegacyTLWithPOJStyleNN("oo6"), "ǒ\u0358");
  EXPECT_EQ(ComposeLegacyTLWithPOJStyleNN("oo7"), "ō\u0358");
  EXPECT_EQ(ComposeLegacyTLWithPOJStyleNN("ooh8"), "o\u030d\u0358h");
  EXPECT_EQ(ComposeLegacyTLWithPOJStyleNN("oo9"), "o\u030b\u0358");
}

TEST(TaiweneseRomanizationTest, UppercaseRegressionTest) {
  EXPECT_EQ(
      Formosa::TaiwaneseRomanization::VowelHelper::uppercasedVowelFromVowel(
          ComposeLegacyPOJ("ou9")),
      ComposeLegacyPOJ("Ou9"));

  EXPECT_EQ(
      Formosa::TaiwaneseRomanization::VowelHelper::uppercasedVowelFromVowel(
          ComposeLegacyTLWithPOJStyleNN("oo9")),
      ComposeLegacyTLWithPOJStyleNN("Oo9"));
}

}
