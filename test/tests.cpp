// Copyright 2021 GHA Test Team

#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <stdexcept>
#include "TimedDoor.h"

using ::testing::Return;

class MockDoor : public Door {
 public:
  MOCK_METHOD(void, lock, (), (override));
  MOCK_METHOD(void, unlock, (), (override));
  MOCK_METHOD(bool, isDoorOpened, (), (override));
};

class MockTimerClient : public TimerClient {
 public:
  MOCK_METHOD(void, Timeout, (), (override));
};

class TimedDoorTest : public ::testing::Test {
 protected:
  TimedDoor* door;

  void SetUp() override {
    door = new TimedDoor(1);
  }

  void TearDown() override {
    delete door;
  }
};

TEST_F(TimedDoorTest, InitiallyDoorIsClosed) {
  EXPECT_FALSE(door->isDoorOpened());
}

TEST_F(TimedDoorTest, LockClosesDoor) {
  door->lock();
  EXPECT_FALSE(door->isDoorOpened());
}

TEST_F(TimedDoorTest, GetTimeOutReturnsCorrectValue) {
  EXPECT_EQ(1, door->getTimeOut());
}

TEST_F(TimedDoorTest, ThrowStateOnClosedDoorDoesNotThrow) {
  EXPECT_NO_THROW(door->throwState());
}

TEST(TimedDoorZeroTimeout, UnlockThrowsWhenDoorRemainOpen) {
  TimedDoor door(0);
  door.lock();
  EXPECT_THROW(door.unlock(), std::runtime_error);
}

TEST(TimedDoorZeroTimeout, DoorStillOpenAfterFailedUnlock) {
  TimedDoor door(0);
  try {
     door.unlock();
     }
  catch (const std::runtime_error&) {}
  EXPECT_THROW(door.throwState(), std::runtime_error);
}

TEST(TimedDoorZeroTimeout, LockAfterFailedUnlockClosesTheDoor) {
  TimedDoor door(0);
  try {
     door.unlock();
     }
  catch (const std::runtime_error&) {}
  door.lock();
  EXPECT_NO_THROW(door.throwState());
}

TEST(DoorTimerAdapterTest, TimeoutOnClosedDoorDoesNotThrow) {
  TimedDoor door(5);
  DoorTimerAdapter adapter(door);
  EXPECT_NO_THROW(adapter.Timeout());
}

TEST(DoorTimerAdapterTest, TimeoutOnOpenDoorThrows) {
  TimedDoor door(0);
  try {
     door.unlock();
    }
  catch (const std::runtime_error&) {}
  DoorTimerAdapter adapter(door);
  EXPECT_THROW(adapter.Timeout(), std::runtime_error);
}

TEST(TimerTest, CallsClientTimeoutExactlyOnce) {
  MockTimerClient client;
  EXPECT_CALL(client, Timeout()).Times(1);
  Timer timer;
  timer.tregister(0, &client);
}

TEST(TimerTest, TimeoutNotCalledWithoutRegister) {
  MockTimerClient client;
  EXPECT_CALL(client, Timeout()).Times(0);
  Timer timer;
}

TEST(MockDoorTest, LockCalledOnce) {
  MockDoor mock;
  EXPECT_CALL(mock, lock()).Times(1);
  mock.lock();
}

TEST(MockDoorTest, IsDoorOpenedReturnsMockedValue) {
  MockDoor mock;
  EXPECT_CALL(mock, isDoorOpened()).Times(1).WillOnce(Return(true));
  EXPECT_TRUE(mock.isDoorOpened());
}

TEST(TimedDoorParamTest, GetTimeOutVariousValues) {
  TimedDoor d10(10);
  TimedDoor d42(42);
  EXPECT_EQ(10, d10.getTimeOut());
  EXPECT_EQ(42, d42.getTimeOut());
}

