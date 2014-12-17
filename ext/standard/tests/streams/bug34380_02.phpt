--TEST--
Bug #34380: stream_select should set posix_globals.last_error on failure
--SKIPIF--
<?php
if (substr(PHP_OS, 0, 3) == "WIN")
	die("skip. Do not run on Windows");
if (!extension_loaded("posix") || !extension_loaded('pcntl'))
	die("skip. posix and pcntl extensions must be loaded");
?>
--FILE--
<?php

function handler($signo) {}
pcntl_signal(SIGUSR2, "handler");

$pid = pcntl_fork();
if ($pid == -1) {
	die('could not fork');
} else if ($pid) {
	$open_spec = array(
		0 => array('pipe', 'r'),
		1 => array('pipe', 'w'),
		2 => array('pipe', 'w')
	);
	$proc = proc_open('/bin/sleep 60', $open_spec, $pipes, NULL, $_ENV);
	$reads = array($pipes[1]);
	$e = NULL;

	// should get EINTR after child sends us a signal
	$fds = stream_select($reads, $e, $e, 60);
	var_dump($fds);
	var_dump(posix_get_last_error());
	proc_terminate($proc);
} else {
	// racy
	sleep(2);
	posix_kill(posix_getppid(), SIGUSR2);
}
?>
--EXPECTF--

Warning: stream_select(): unable to select [4]: Interrupted system call (max_fd=%d) in %s on line %d
bool(false)
int(4)
