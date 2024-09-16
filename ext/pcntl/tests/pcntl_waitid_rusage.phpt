--TEST--
pcntl_waitid() and rusage
--EXTENSIONS--
pcntl
posix
--SKIPIF--
<?php
if (!function_exists('pcntl_waitid')) die('skip pcntl_waitid unavailable');
if (!str_contains(PHP_OS, 'Linux') && !str_contains(PHP_OS, 'FreeBSD')) {
    die('skip pcntl_waitid can return rusage only on FreeBSD and Linux');
}
?>
--FILE--
<?php
$pid = pcntl_fork();
if ($pid == -1) {
    die('failed');
} else if ($pid) {
    var_dump(pcntl_waitid(P_PID, $pid, $siginfo, WSTOPPED, $rusage));
    var_dump($rusage['ru_utime.tv_sec']);
    var_dump($rusage['ru_utime.tv_usec']);

    $rusage = array(1,2,3);
    posix_kill($pid, SIGCONT);
    var_dump(pcntl_waitid(P_PID, $pid, $siginfo, WCONTINUED, $rusage));
    var_dump($rusage['ru_utime.tv_sec']);
    var_dump($rusage['ru_utime.tv_usec']);

    posix_kill($pid, SIGUSR1);
    var_dump(pcntl_waitid(P_PID, $pid, $siginfo, WEXITED, $rusage));
    var_dump($rusage['ru_maxrss']);
    var_dump($siginfo['status']);

    $rusage = 'string';
    var_dump(pcntl_waitid(P_PID, $pid, $siginfo, WEXITED | WNOHANG, $rusage));
    var_dump(gettype($rusage));
    var_dump(count($rusage));

    $rusage = new stdClass;
    var_dump(pcntl_waitid(P_PID, $pid, $siginfo, WEXITED | WNOHANG, $rusage));
    var_dump(gettype($rusage));
    var_dump(count($rusage));

    fwrite(STDOUT, 'END' . PHP_EOL);
} else {
    pcntl_signal(SIGUSR1, function ($_signo, $_siginfo) { exit(42); });
    posix_kill(posix_getpid(), SIGSTOP);
    $nanoseconds = 100;
    while (true) {
        pcntl_signal_dispatch();
        time_nanosleep(0, $nanoseconds);
        $nanoseconds *= 2;
    }
}
?>
--EXPECTF--
bool(true)
int(%d)
int(%d)
bool(true)
int(%d)
int(%d)
bool(true)
int(%d)
int(42)
bool(false)
string(5) "array"
int(0)
bool(false)
string(5) "array"
int(0)
END
