// credits to: https://stackoverflow.com/questions/77005/how-to-automatically-generate-a-stacktrace-when-my-program-crashes

#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif
#ifndef __USE_GNU
#define __USE_GNU
#endif

#include <cstdint>
#include <iostream>

#include <cxxabi.h>
#include <execinfo.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ucontext.h>
#include <unistd.h>

typedef struct _sigcontext_64 {
	uint64_t r8;
	uint64_t r9;
	uint64_t r10;
	uint64_t r11;
	uint64_t r12;
	uint64_t r13;
	uint64_t r14;
	uint64_t r15;
	uint64_t di;
	uint64_t si;
	uint64_t bp;
	uint64_t bx;
	uint64_t dx;
	uint64_t ax;
	uint64_t cx;
	uint64_t sp;
	uint64_t ip;
	uint64_t flags;
	uint16_t cs;
	uint16_t gs;
	uint16_t fs;
	uint16_t ss;
	uint64_t err;
	uint64_t trapno;
	uint64_t oldmask;
	uint64_t cr2;
	uint64_t fpstate;
	uint64_t reserved1[8];
} sigcontext_64;

/* This structure mirrors the one found in /usr/include/asm/ucontext.h */
typedef struct _sig_ucontext1 {
	unsigned long     uc_flags;
	ucontext_t        *uc_link;
	stack_t           uc_stack;
	sigcontext_64     uc_mcontext;
	sigset_t          uc_sigmask;
} sig_ucontext_t;

void crit_err_hdlr(int sig_num, siginfo_t * info, void * ucontext)
{
    sig_ucontext_t * uc = (sig_ucontext_t *)ucontext;

    void * caller_address = (void *) uc->uc_mcontext.ip;

    std::cerr << "signal " << sig_num 
              << " (" << strsignal(sig_num) << "), address is " 
              << info->si_addr << " from " << caller_address 
              << std::endl << std::endl;

    void * array[50];
    int size = backtrace(array, 50);

    array[1] = caller_address;

    char ** messages = backtrace_symbols(array, size);    

    // skip first stack frame (points here)
    for (int i = 1; i < size && messages != NULL; ++i)
    {
        char *mangled_name = 0, *offset_begin = 0, *offset_end = 0;

        // find parantheses and +address offset surrounding mangled name
        for (char *p = messages[i]; *p; ++p)
        {
            if (*p == '(') 
            {
                mangled_name = p; 
            }
            else if (*p == '+') 
            {
                offset_begin = p;
            }
            else if (*p == ')')
            {
                offset_end = p;
                break;
            }
        }

        // if the line could be processed, attempt to demangle the symbol
        if (mangled_name && offset_begin && offset_end && 
            mangled_name < offset_begin)
        {
            *mangled_name++ = '\0';
            *offset_begin++ = '\0';
            *offset_end++ = '\0';

            int status;
            char * real_name = abi::__cxa_demangle(mangled_name, 0, 0, &status);

            // if demangling is successful, output the demangled function name
            if (status == 0)
            {    
                std::cerr << "[bt]: (" << i << ") " << messages[i] << " : " 
                          << real_name << "+" << offset_begin << offset_end 
                          << std::endl;

            }
            // otherwise, output the mangled function name
            else
            {
                std::cerr << "[bt]: (" << i << ") " << messages[i] << " : " 
                          << mangled_name << "+" << offset_begin << offset_end 
                          << std::endl;
            }
            free(real_name);
        }
        // otherwise, print the whole line
        else
        {
            std::cerr << "[bt]: (" << i << ") " << messages[i] << std::endl;
        }
    }
    std::cerr << std::endl;

    free(messages);

    exit(EXIT_FAILURE);
}

void setupSigAct()
{
	struct sigaction sigact;

	sigact.sa_sigaction = crit_err_hdlr;
	sigact.sa_flags = SA_RESTART | SA_SIGINFO;

	if (sigaction(SIGSEGV, &sigact, (struct sigaction *)NULL) != 0)
	{
		fprintf(stderr, "error setting signal handler for %d (%s)\n", SIGSEGV, strsignal(SIGSEGV));

		exit(EXIT_FAILURE);
	}
}
