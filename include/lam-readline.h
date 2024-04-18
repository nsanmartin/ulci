#ifndef __LAM_READLINE_H_
#define __LAM_READLINE_H_

/*
 * This file is for building in systems with no readline
 */

char* lam_readline (const char *prompt);

#ifdef NO_READLINE
static inline void add_history(char *s) { (void)s;}
static inline char* readline (const char *prompt) { return lam_readline(prompt); }
#endif
#endif
