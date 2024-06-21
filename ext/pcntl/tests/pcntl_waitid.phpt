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
    // invalid idtype
    var_dump(pcntl_waitid(-42, $pid, $siginfo, WSTOPPED));
    // invalid flags
    var_dump(pcntl_waitid(P_PID, $pid, $siginfo, -42));
    // need at least one of WEXITED, WSTOPPED, WCONTINUED flagd
    var_dump(pcntl_waitid(P_PID, $pid, $siginfo, WNOHANG));

    // with WNOHANG, call succeeds but there is no PID state change
    $result = pcntl_waitid(P_PID, $pid, $siginfo, WNOHANG | WEXITED);
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
Warning: pcntl_waitid(): Invalid argument in %s on line 7
bool(false)

Warning: pcntl_waitid(): Invalid argument in %s on line 9
bool(false)

Warning: pcntl_waitid(): Invalid argument in %s on line 11
bool(false)
bool(true)
int(0)
bool(true)
bool(true)
bool(true)
int(42)
