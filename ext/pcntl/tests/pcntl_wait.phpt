--TEST--
pcntl_wait()
--SKIPIF--
<?php if (!extension_loaded("posix")) die("skip posix extension not available"); ?>
<?php 
--FILE--
<?php
$pid = pcntl_fork();
if ($pid == 1) {
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
	var_dump(pcntl_wait());
	var_dump(pcntl_waitpid());

	var_dump(pcntl_wifexited());
	var_dump(pcntl_wifstopped());
	var_dump(pcntl_wifsignaled());
	var_dump(pcntl_wexitstatus());
	var_dump(pcntl_wtermsig());
	var_dump(pcntl_wstopsig());
} else {
	posix_kill(posix_getpid(), SIGSTOP);
	exit(42);
}
?>
--EXPECTF--
bool(false)
bool(false)
bool(false)
int(42)
int(-1)

Warning: pcntl_wait() expects at least 1 parameter, 0 given in %s
NULL

Warning: pcntl_waitpid() expects at least 2 parameters, 0 given in %s
NULL

Warning: pcntl_wifexited() expects exactly 1 parameter, 0 given in %s
NULL

Warning: pcntl_wifstopped() expects exactly 1 parameter, 0 given in %s
NULL

Warning: pcntl_wifsignaled() expects exactly 1 parameter, 0 given in %s
NULL

Warning: pcntl_wexitstatus() expects exactly 1 parameter, 0 given in %s
NULL

Warning: pcntl_wtermsig() expects exactly 1 parameter, 0 given in %s
NULL

Warning: pcntl_wstopsig() expects exactly 1 parameter, 0 given in %s
NULL
