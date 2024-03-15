#include "lib.h"
#include <gtest/gtest.h>

TEST(BasicTest, TestVersion	) {
	ASSERT_GT(version::get(), 0);
}
