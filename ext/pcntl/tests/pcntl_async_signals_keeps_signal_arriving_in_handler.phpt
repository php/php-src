--TEST--
pcntl_async_signals(): a signal that arrives while a handler runs is dispatched after it
--EXTENSIONS--
pcntl
posix
--FILE--
<?php

pcntl_async_signals(true);

pcntl_signal(SIGHUP, function () {
    echo "HUP\n";
});
pcntl_sigprocmask(SIG_BLOCK, [SIGHUP]);
posix_kill(posix_getpid(), SIGHUP);

pcntl_signal(SIGUSR1, function () {
    echo "USR1\n";
    pcntl_sigprocmask(SIG_UNBLOCK, [SIGHUP]);
    echo "Unblocked\n";
});

posix_kill(posix_getpid(), SIGUSR1);
// A check point for the interrupt the dispatch raised again: the return of an internal call.
posix_getpid();
echo "Done\n";

?>
--EXPECT--
USR1
Unblocked
HUP
Done
