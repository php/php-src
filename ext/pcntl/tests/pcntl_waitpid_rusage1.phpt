--TEST--
pcntl_waitpid() and rusage
--SKIPIF--
<?php if (!extension_loaded("pcntl")) print "skip"; ?>
<?php if (!extension_loaded("posix")) die("skip posix extension not available"); ?>
--FILE--
<?php
$pid = pcntl_fork();
if ($pid == -1) {
	die("failed");
} else if ($pid) {
	$status = 0;
	var_dump(pcntl_waitpid($pid, $status, WUNTRACED, $rusage));
	var_dump($rusage['ru_utime.tv_sec']);
	var_dump($rusage['ru_utime.tv_usec']);

	posix_kill($pid, SIGCONT);

	$rusage = array(1,2,3);
	pcntl_waitpid($pid, $status, WUNTRACED, $rusage);
	var_dump($rusage['ru_utime.tv_sec']);
	var_dump($rusage['ru_utime.tv_usec']);

	$rusage = "string";
	pcntl_waitpid($pid, $status, 0, $rusage);
	var_dump(gettype($rusage));
	var_dump(count($rusage));

	$rusage = new stdClass;
	pcntl_waitpid($pid, $status, 0, $rusage);
	var_dump(gettype($rusage));
	var_dump(count($rusage));

	echo "END\n";
} else {
	posix_kill(posix_getpid(), SIGSTOP);
	exit(42);
}
?>
--EXPECTF--
int(%d)
int(%d)
int(%d)
int(%d)
int(%d)
string(5) "array"
int(0)
string(5) "array"
int(0)
END
