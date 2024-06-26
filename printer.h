#pragma once

#include "common.h"

class LogPrinter : public Listener {
public:
    void print(const CommandBlock& block) override;
};

class FilePrinter : public Listener {
public:
    void print(const CommandBlock& block) override;
};