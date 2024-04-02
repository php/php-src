--TEST--
pcntl: pcntl_sigprocmask(), pcntl_sigwaitinfo(), pcntl_sigtimedwait()
--EXTENSIONS--
pcntl
posix
--SKIPIF--
<?php
if (
    !function_exists('pcntl_sigprocmask')
    or !function_exists('pcntl_sigwaitinfo')
    or !function_exists('pcntl_sigtimedwait')
) { die('skip required functionality is not available'); }
elseif (!defined('CLD_EXITED')) die('skip CLD_EXITED not defined');
elseif (getenv("SKIP_REPEAT")) die("skip cannot be repeated");
elseif (str_contains(PHP_OS, 'FreeBSD')) die('skip Results in parallel test runner hang on FreeBSD');
?>
--FILE--
<?php

$pid = pcntl_fork();
if ($pid == -1) {
    die('failed');
} else if ($pid) {
    pcntl_sigprocmask(SIG_BLOCK, array(SIGCHLD,(string)SIGTERM));
    $oldset = array();
    pcntl_sigprocmask(SIG_UNBLOCK, array(SIGINT), $oldset);
    var_dump(in_array(SIGCHLD, $oldset));
    var_dump(in_array(SIGTERM, $oldset));

    posix_kill(posix_getpid(), SIGTERM);
    $signo = pcntl_sigwaitinfo(array(SIGTERM), $siginfo);
    echo "signo == SIGTERM\n";
    var_dump($signo === SIGTERM && $signo === $siginfo['signo']);
    echo "code === SI_USER || SI_NOINFO\n";
    if (defined('SI_NOINFO')) {
        var_dump(($siginfo['code'] === SI_USER) || ($siginfo['code'] === SI_NOINFO));
    } else {
        var_dump($siginfo['code'] === SI_USER);
    }

    pcntl_signal(SIGCHLD, function($signo){});
    posix_kill($pid, SIGTERM);
    $signo = pcntl_sigwaitinfo(array((string)SIGCHLD), $siginfo);
    echo "signo == SIGCHLD\n";
    var_dump($signo === SIGCHLD && $signo === $siginfo['signo']);
    echo "code === CLD_KILLED\n";
    var_dump($siginfo['code'] === CLD_KILLED);
    echo "signo === SIGCHLD\n";
    var_dump($siginfo['signo'] === SIGCHLD);
    echo "signo === uid\n";
    var_dump($siginfo['uid'] === posix_getuid());
    echo "signo === pid\n";
    var_dump($siginfo['pid'] === $pid);
    pcntl_waitpid($pid, $status);
} else {
    $siginfo = NULL;
    pcntl_sigtimedwait(array(SIGINT), $siginfo, 3600, 0);
    exit;
}

?>
--EXPECT--
bool(true)
bool(true)
signo == SIGTERM
bool(true)
code === SI_USER || SI_NOINFO
bool(true)
signo == SIGCHLD
bool(true)
code === CLD_KILLED
bool(true)
signo === SIGCHLD
bool(true)
signo === uid
bool(true)
signo === pid
bool(true)
