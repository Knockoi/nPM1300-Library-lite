# nPM1300 Arduino Library - I2C Communication Guide

This project explains how to use I2C communication in an Arduino library for the Nordic nPM1300 Power Management IC (PMIC).  
It shows how the library initializes I2C, reads, and writes registers through the bus.

---

## Overview

The nPM1300 communicates with the host microcontroller (such as Arduino or nRF52840) using the I2C protocol.  
This guide demonstrates how the library wraps low-level I2C operations into easy-to-use functions.

---

## I2C Basics

I2C uses two lines:

- **SDA**: Data line  
- **SCL**: Clock line  

Structure:

- Master–Slave communication (Arduino = master, nPM1300 = slave)
- Each device has a 7-bit address, e.g. `0x6B`

---

## Library Structure

### Initialization

```cpp
// NPM1300.h
class NPM1300 {
private:
    TwoWire *_i2c;
};

// NPM1300.cpp
NPM1300::NPM1300(TwoWire &wirePort) {
    _i2c = &wirePort;
}

bool NPM1300::begin() {
    _i2c->begin();
    if (!isConnected()) return false;
    return true;
}
