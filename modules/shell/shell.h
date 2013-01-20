// eLua shell 

#ifndef __SHELL_H__
#define __SHELL_H__

#define ELUA_STR_VERSION    "5.2.1"
#define SHELL_WELCOMEMSG    "\nscmRtos %s \n"
#define SHELL_PROMPT        "scm> "
#define SHELL_ERRMSG        "Invalid command, type 'help' for help\n"
#define SHELL_MAXSIZE       50
#define SHELL_MAX_LUA_ARGS  8

int shell_init();
void shell_start();

#endif // #ifndef __SHELL_H__
