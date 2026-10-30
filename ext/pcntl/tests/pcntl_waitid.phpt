--TEST--
pcntl_waitid()
--EXTENSIONS--
pcntl
posix
--SKIPIF--
<?php
    if (!function_exists('pcntl_waitid')) die('skip pcntl_waitid unavailable');
?>
--FILE--
<?php
$pid = pcntl_fork();
if ($pid == -1) {
    die("failed");
} else if ($pid) {
    // invalid flags
    var_dump(pcntl_waitid(P_PID, $pid, $siginfo, 0));
    var_dump(pcntl_get_last_error() == PCNTL_EINVAL);

    var_dump(pcntl_waitid(P_PID, $pid, $siginfo, WSTOPPED));
    posix_kill($pid, SIGCONT);
    var_dump(pcntl_waitid(P_PID, $pid, $siginfo, WCONTINUED));
    posix_kill($pid, SIGUSR1);
    var_dump(pcntl_waitid(P_PID, $pid, $siginfo, WEXITED));
    var_dump($siginfo["status"]);
} else {
    pcntl_signal(SIGUSR1, function ($_signo, $_siginfo) { exit(42); });
    posix_kill(posix_getpid(), SIGSTOP);
    pcntl_signal_dispatch();
    sleep(42);
    pcntl_signal_dispatch();
    exit(6);
}
?>
--EXPECTF--
bool(false)
bool(true)
bool(true)
bool(true)
bool(true)
int(42)
