--TEST--
pcntl_signal_dispatch() keeps a signal that arrives in a throwing handler after the ones that handler left
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
    pcntl_sigprocmask(SIG_UNBLOCK, [SIGHUP]);
    throw new Exception("USR1");
});
pcntl_signal(SIGUSR2, function () {
    echo "USR2\n";
});

posix_kill(posix_getpid(), SIGUSR1);
posix_kill(posix_getpid(), SIGUSR2);

try {
    pcntl_signal_dispatch();
} catch (Exception $e) {
    echo $e->getMessage(), "\n";
}
posix_kill(posix_getpid(), SIGUSR2);
pcntl_signal_dispatch();

?>
--EXPECT--
USR1
USR2
HUP
USR2
