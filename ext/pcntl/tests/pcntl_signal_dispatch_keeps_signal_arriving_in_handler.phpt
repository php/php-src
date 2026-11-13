--TEST--
pcntl_signal_dispatch() keeps a signal that arrives while its handlers run
--EXTENSIONS--
pcntl
posix
--FILE--
<?php

pcntl_signal(SIGHUP, function () {
    echo "HUP\n";
});
pcntl_sigprocmask(SIG_BLOCK, [SIGHUP]);
posix_kill(posix_getpid(), SIGHUP);

pcntl_signal(SIGUSR1, function () {
    echo "USR1\n";
    pcntl_sigprocmask(SIG_UNBLOCK, [SIGHUP]);
});

posix_kill(posix_getpid(), SIGUSR1);
pcntl_signal_dispatch();
echo "Dispatched\n";
pcntl_signal_dispatch();

?>
--EXPECT--
USR1
Dispatched
HUP
