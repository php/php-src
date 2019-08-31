--TEST--
Test posix_kill(), posix_get_last_error and posix_strerror() functions : basic functionality
--SKIPIF--
<?php
	if (!extension_loaded('posix')) die('skip - POSIX extension not loaded');
?>
--FILE--
<?php
  echo "Basic test of POSIX getpgid(), kill(), get_last_error() and strerror() functions\n";

  	// Don't rely on PCNTL extension being around
  	$SIGKILL = 9;

  	// TODO Once we have PS open working beef up this test to create a process and kill it
  	// for now start at a low pid and find first pid which does not exist.
  	$pid = 999;
  	do {
  		$pid += 1;
  		$result = shell_exec("ps -p " . $pid);
	} while (stripos($result, (string)$pid) != FALSE);

  	echo "Kill pid=" . $pid . "\n";
	var_dump(posix_kill($pid,$SIGKILL));

	$errno = posix_get_last_error();

	var_dump($errno);
	var_dump(posix_strerror($errno));

?>
===DONE====
--EXPECTF--
Basic test of POSIX getpgid(), kill(), get_last_error() and strerror() functions
Kill pid=%d
bool(false)
int(%d)
string(%d) %s%rNo such process|Operation not permitted%r%s
===DONE====
