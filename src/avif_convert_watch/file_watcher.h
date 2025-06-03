//
// Created by Ramon on 3/06/2025.
//

#ifndef FILE_WATCHER_H
#define FILE_WATCHER_H

void file_watcher_start(const char *root, void(*callback)(const char *path));

int is_directory(const char *path);

#endif //FILE_WATCHER_H
