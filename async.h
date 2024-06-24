#pragma once

#include <string_view>

int connect(int blockSize); //returns context id
void receive(int id, std::string_view buf);
void disconnect(int id);