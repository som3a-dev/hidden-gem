/*
* This module (dialog.h/c) is a platform layer abstraction for some platform specific native dialogs
* For example the file selection dialog on windows, mac and linux
*/

#ifndef _DIALOG_H
#define _DIALOG_H

/*
* Opens up a file selection dialog for the user
* Returns the selected file's path as a heap allocated string, caller must free()
*/
char* dialog_select_file();

#endif