#ifndef EXPLORER_H
#define EXPLORER_H

#include <stdio.h>
#include <windows.h>
#include <conio.h>
#include <dirent.h>

#define KEY_UP 72
#define KEY_DOWN 80
#define KEY_ENTER 13
#define KEY_ESC 27
#define KEY_CTRL_Q 17

#define COLOR_DEFAULT 7
#define COLOR_SELECTED 112
#define COLOR_DIRECTORY 9

typedef struct {
    char name[256];
    int isDir;
    long long fileSize;
    char creationDate[20];
    char modificationDate[20];
} FileEntry;

extern FileEntry entries[256];
extern int totalEntries;
extern int currentSelection;
extern char currentPath[MAX_PATH];

void setCursorVisibility(int visible);
void setConsoleColor(int color);
void moveCursor(int x, int y);
void clearLine();
void formatFileSize(long long size, char* out);
void getFileInfo(const char *path, FileEntry *entry);
void drawDirectory();
void updateSelection();
void loadDirectory(const char *path);
void navigateUp();
void handleEnter();
char* startExplorer();

#endif // EXPLORER_H
