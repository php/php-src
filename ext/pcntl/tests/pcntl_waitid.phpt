--TEST--
pcntl_waitid()
--EXTENSIONS--
pcntl
posix
--FILE--
<?php
$pid = pcntl_fork();
if ($pid == -1) {
    die("failed");
} else if ($pid) {
    $status = 0;
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
bool(true)
bool(true)
bool(true)
int(42)
