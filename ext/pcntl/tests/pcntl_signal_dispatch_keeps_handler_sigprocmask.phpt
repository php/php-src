--TEST--
pcntl_signal_dispatch() runs its handlers under the thread's mask and keeps the changes they made
--EXTENSIONS--
pcntl
posix
--FILE--
<?php

function blocked(): array {
    // SIGCHLD, unused here: the call refuses an empty list.
    pcntl_sigprocmask(SIG_BLOCK, [SIGCHLD], $mask);
    pcntl_sigprocmask(SIG_SETMASK, $mask);
    return $mask;
}

function is_blocked(int $signo): bool {
    return in_array($signo, blocked(), true);
}

pcntl_sigprocmask(SIG_BLOCK, [SIGHUP, SIGALRM]);

pcntl_signal(SIGUSR1, function () {
    pcntl_sigprocmask(SIG_BLOCK, [SIGUSR2]);
    pcntl_sigprocmask(SIG_UNBLOCK, [SIGHUP]);
});

posix_kill(posix_getpid(), SIGUSR1);
pcntl_signal_dispatch();

var_dump(is_blocked(SIGUSR2));
var_dump(is_blocked(SIGHUP));
var_dump(is_blocked(SIGALRM));

echo "Saved and restored in a handler\n";

$before = blocked();

pcntl_signal(SIGUSR1, function () {
    pcntl_sigprocmask(SIG_BLOCK, [SIGTERM], $old);
    pcntl_sigprocmask(SIG_SETMASK, $old);
});

posix_kill(posix_getpid(), SIGUSR1);
pcntl_signal_dispatch();

var_dump(blocked() === $before);

echo "Changed by the destructor of the handler's result\n";

class Result {
    public function __destruct() {
        pcntl_sigprocmask(SIG_BLOCK, [SIGUSR2]);
    }
}

pcntl_sigprocmask(SIG_UNBLOCK, [SIGUSR2]);
pcntl_signal(SIGUSR1, function () {
    return new Result;
});

posix_kill(posix_getpid(), SIGUSR1);
pcntl_signal_dispatch();

var_dump(is_blocked(SIGUSR2));

?>
--EXPECT--
bool(true)
bool(false)
bool(true)
Saved and restored in a handler
bool(true)
Changed by the destructor of the handler's result
bool(true)
