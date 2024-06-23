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
    try {
        pcntl_waitid(P_PID, $pid, $siginfo, -42);
    } catch (\ValueError $e) {
        echo $e->getMessage() . \PHP_EOL;
    }
    try {
        pcntl_waitid(P_PID, $pid, $siginfo, PHP_INT_MAX);
    } catch (\ValueError $e) {
        echo $e->getMessage() . \PHP_EOL;
    }
    // need at least one of WEXITED, WSTOPPED, WCONTINUED flags
    try {
        pcntl_waitid(P_PID, $pid, $siginfo, WNOHANG);
    } catch (\ValueError $e) {
        echo $e->getMessage() . \PHP_EOL;
    }

    // with WNOHANG, call succeeds but there is no PID state change
    $result = pcntl_waitid(P_PID, $pid, $siginfo, WEXITED | WNOHANG);
    var_dump($result);
    var_dump($siginfo["pid"]);

    $result = pcntl_waitid(P_PID, $pid, $siginfo, WSTOPPED);
    var_dump($result);

    posix_kill($pid, SIGCONT);
    $result = pcntl_waitid(P_PID, $pid, $siginfo, WCONTINUED);
    var_dump($result);

    $result = pcntl_waitid(P_PID, $pid, $siginfo, WEXITED);
    var_dump($result);
    var_dump($siginfo["status"]);
} else {
    posix_kill(posix_getpid(), SIGSTOP);
    exit(42);
}
?>
--EXPECTF--
An invalid value was specified for options, or idtype and id specify an invalid set of processes
An invalid value was specified for options, or idtype and id specify an invalid set of processes
An invalid value was specified for options, or idtype and id specify an invalid set of processes
bool(true)
int(0)
bool(true)
bool(true)
bool(true)
int(42)
