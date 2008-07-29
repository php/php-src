--TEST--
pcntl: pcntl_sigprocmask(), pcntl_sigwaitinfo(), pcntl_sigtimedwait()
--SKIPIF--
<?php
if (!extension_loaded('pcntl')) die('skip pcntl extension not available');
if (!extension_loaded('posix')) die('skip posix extension not available');
?>
--FILE--
<?php

$pid = pcntl_fork();
if ($pid == -1) {
	die('failed');
} else if ($pid) {
	pcntl_sigprocmask(SIG_BLOCK, array(SIGCHLD,(string)SIGTERM));
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

	echo "sigprocmask with invalid arguments\n";
	var_dump(pcntl_sigprocmask(PHP_INT_MAX, array(SIGTERM)));
	var_dump(pcntl_sigprocmask(SIG_SETMASK, array(0)));

	echo "sigwaitinfo with invalid arguments\n";
	var_dump(pcntl_sigwaitinfo(array(0)));

	echo "sigtimedwait with invalid arguments\n";
	var_dump(pcntl_sigtimedwait(array(SIGTERM), $signo, PHP_INT_MAX, PHP_INT_MAX));
} else {
	$siginfo = NULL;
	pcntl_sigtimedwait(array(SIGTERM), $siginfo, PHP_INT_MAX, 999999999);
	exit;
}

?>
--EXPECTF--
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
sigprocmask with invalid arguments

Warning: pcntl_sigprocmask(): Invalid argument in %s on line %d
bool(false)

Warning: pcntl_sigprocmask(): Invalid argument in %s on line %d
bool(false)
sigwaitinfo with invalid arguments

Warning: pcntl_sigwaitinfo(): Invalid argument in %s on line %d
bool(false)
sigtimedwait with invalid arguments

Warning: pcntl_sigtimedwait(): Invalid argument in %s on line %d
int(-1)
