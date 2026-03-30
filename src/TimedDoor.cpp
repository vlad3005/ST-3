// Copyright 2021 GHA Test Team
#include "TimedDoor.h"
#include <stdexcept>
#include <thread>
#include <chrono>



void Timer::sleep(int timeout) {
  std::this_thread::sleep_for(std::chrono::seconds(timeout));
}

void Timer::tregister(int timeout, TimerClient* c) {
  client = c;
  sleep(timeout);
  client->Timeout();
}

DoorTimerAdapter::DoorTimerAdapter(TimedDoor& d) : door(d) {}

void DoorTimerAdapter::Timeout() {
  door.throwState();
}

TimedDoor::TimedDoor(int timeout) : iTimeout(timeout), isOpened(false) {
  adapter = new DoorTimerAdapter(*this);
}

TimedDoor::~TimedDoor() {
  delete adapter;
}

bool TimedDoor::isDoorOpened() {
  return isOpened;
}

void TimedDoor::lock() {
  isOpened = false;
}

void TimedDoor::unlock() {
  isOpened = true;
  Timer timer;
  timer.tregister(iTimeout, adapter);
}

int TimedDoor::getTimeOut() const {
  return iTimeout;
}

void TimedDoor::throwState() {
  if (isOpened) {
    throw std::runtime_error("Door is open after timeout!");
  }
}

