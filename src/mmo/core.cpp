#include "core.hpp"
//    core.cpp - The main loop.
//
//    Copyright © ????-2004 Athena Dev Teams
//    Copyright © 2004-2011 The Mana World Development Team
//    Copyright © 2011-2014 Ben Longbons <b.r.longbons@gmail.com>
//
//    This file is part of The Mana World (Athena server)
//
//    This program is free software: you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation, either version 3 of the License, or
//    (at your option) any later version.
//
//    This program is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with this program.  If not, see <http://www.gnu.org/licenses/>.

#include <sys/wait.h>

#include <unistd.h>

#include <csignal>
#include <cstdlib>
#include <ctime>

#include "../strings/zstring.hpp"

#include "../generic/random.hpp"

#include "../io/cxxstdio.hpp"

#include "socket.hpp"
#include "timer.hpp"

#include "../poison.hpp"

// Added by Gabuzomeu
//
// This is an implementation of signal() using sigaction() for portability.
// (sigaction() is POSIX; signal() is not.)  Taken from Stevens' _Advanced
// Programming in the UNIX Environment_.
//
typedef void(*sigfunc)(int);
static
sigfunc compat_signal(int signo, sigfunc func)
{
    struct sigaction sact, oact;

    sact.sa_handler = func;
    sigfillset(&sact.sa_mask);
    sigdelset(&sact.sa_mask, SIGSEGV);
    sigdelset(&sact.sa_mask, SIGBUS);
    sigdelset(&sact.sa_mask, SIGTRAP);
    sigdelset(&sact.sa_mask, SIGILL);
    sigdelset(&sact.sa_mask, SIGFPE);
    sact.sa_flags = 0;

    if (sigaction(signo, &sact, &oact) < 0)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wold-style-cast"
        return SIG_ERR;
#pragma GCC diagnostic pop

    return oact.sa_handler;
}

volatile
bool runflag = true;

static
void chld_proc(int)
{
    wait(NULL);
}
static
void sig_proc(int)
{
    for (int i = 1; i < 31; ++i)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wold-style-cast"
        compat_signal(i, SIG_IGN);
#pragma GCC diagnostic pop
    runflag = false;
}

/*
    Note about fatal signals:

    Under certain circumstances,
    the following signals MUST not be ignored:
    SIGFPE, SIGSEGV, SIGILL
    Unless you use SA_SIGINFO and *carefully* check the origin,
    that means they must be SIG_DFL.
 */
int main(int argc, char **argv)
{
    // ZString args[argc]; is (deliberately!) not supported by clang yet
    ZString *args = static_cast<ZString *>(alloca(argc * sizeof(ZString)));
    for (int i = 0; i < argc; ++i)
        args[i] = ZString(strings::really_construct_from_a_pointer, argv[i], nullptr);
    do_init(Slice<ZString>(args, argc));

    if (!runflag)
    {
        PRINTF("Fatal error during startup; exiting\n");
        return 1;
    }
    // set up exit handlers *after* the initialization has happened.
    // This is because term_func is likely to depend on successful init.

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wold-style-cast"
    compat_signal(SIGPIPE, SIG_IGN);
#pragma GCC diagnostic pop
    compat_signal(SIGTERM, sig_proc);
    compat_signal(SIGINT, sig_proc);
    compat_signal(SIGCHLD, chld_proc);

    // Signal to create coredumps by system when necessary (crash)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wold-style-cast"
    compat_signal(SIGSEGV, SIG_DFL);
    compat_signal(SIGBUS, SIG_DFL);
    compat_signal(SIGTRAP, SIG_DFL);
    compat_signal(SIGILL, SIG_DFL);
    compat_signal(SIGFPE, SIG_DFL);
#pragma GCC diagnostic pop

    atexit(term_func);

    while (runflag)
    {
        // TODO - if timers take a long time to run, this
        // may wait too long in sendrecv
        tick_t now = milli_clock::now();
        interval_t next = do_timer(now);
        do_sendrecv(next);
        do_parsepacket();
    }
}