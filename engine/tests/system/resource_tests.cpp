#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <system/resource.hpp>

TEST(resource_tests,CLASS_ID) {
    EXPECT_EQ(compile_time_hash("sigmafive::system::resource"),sigmafive::system::resource::CLASS_ID);
}