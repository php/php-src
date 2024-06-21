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
    try {
        var_dump(pcntl_waitid(-42, $pid, $siginfo, WSTOPPED));
    } catch (\ValueError $e) {
        echo $e->getMessage() . \PHP_EOL;
    }
    var_dump(pcntl_waitid(PHP_INT_MAX, $pid, $siginfo, WSTOPPED));
    // invalid flags
    var_dump(pcntl_waitid(P_PID, $pid, $siginfo, -42));
    var_dump(pcntl_waitid(P_PID, $pid, $siginfo, PHP_INT_MAX));
    // need at least one of WEXITED, WSTOPPED, WCONTINUED flagd
    var_dump(pcntl_waitid(P_PID, $pid, $siginfo, WNOHANG));

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
pcntl_waitid(): Argument #1 ($idtype) must be either one of P_ALL, P_PID, P_PGID (POSIX) or a platform-specific value

Warning: pcntl_waitid(): FOO Invalid argument FOO in /home/random/github/php-src/ext/pcntl/tests/pcntl_waitid.php on line 12
bool(false)

Warning: pcntl_waitid(): FOO Invalid argument FOO in /home/random/github/php-src/ext/pcntl/tests/pcntl_waitid.php on line 14
bool(false)

Warning: pcntl_waitid(): FOO Invalid argument FOO in /home/random/github/php-src/ext/pcntl/tests/pcntl_waitid.php on line 15
bool(false)

Warning: pcntl_waitid(): FOO Invalid argument FOO in /home/random/github/php-src/ext/pcntl/tests/pcntl_waitid.php on line 17
bool(false)
bool(true)
int(0)
bool(true)
bool(true)
bool(true)
int(42)
