--TEST--
pcntl_wait()
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
    pcntl_wait($status, WUNTRACED);
    var_dump(pcntl_wifexited($status));
    posix_kill($pid, SIGCONT);

    pcntl_wait($status);
    var_dump(pcntl_wifsignaled($status));
    var_dump(pcntl_wifstopped($status));
    var_dump(pcntl_wexitstatus($status));

    var_dump(pcntl_wait($status, WNOHANG | WUNTRACED));
} else {
    posix_kill(posix_getpid(), SIGSTOP);
    exit(42);
}
?>
--EXPECT--
bool(false)
bool(false)
bool(false)
int(42)
int(-1)
