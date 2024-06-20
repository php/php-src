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
    // test invalid arguments
    var_dump(@pcntl_waitid(6, $pid, $siginfo, WSTOPPED));
    var_dump(@pcntl_waitid(P_PID, $pid, $siginfo, WNOHANG));

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
--EXPECT--
bool(false)
bool(false)
bool(true)
bool(true)
bool(true)
int(42)
