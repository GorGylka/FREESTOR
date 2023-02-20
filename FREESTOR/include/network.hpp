#pragma once

#include <string>

#define SAVE_FILE "ms0:/pkg/game.pkg"

int drawNetDialog();
int netDialog();
void stopNetworking();
void startNetworking();

int curlDownload(std::string &url, std::string file = SAVE_FILE);

size_t write_data(void *ptr, size_t size, size_t nmemb, void *stream);