#ifndef CORE_TEST_H
#define CORE_TEST_H

#include <gtest/gtest.h>
#include "core/core.h"

class CoreTestFixture : public ::testing::Test {
protected:
    void SetUp() override {
        core_handler.setReady(true);
    }

    CoreModule::CoreHandler core_handler;
};

#endif // CORE_TEST_H
