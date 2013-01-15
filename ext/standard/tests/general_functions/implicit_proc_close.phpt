--TEST--
Implicit proc_close()s (by the GC) should not wait for the process to exit.
--SKIPIF--
<?php
	if(!is_executable("/bin/sleep")) {
		echo "skip";
	}
	if(!function_exists("proc_open")) {
		echo "skip proc_open() is not available";
	}
?>
--FILE--
<?php
	$descr = array(
		0 => array("pipe", "r"),
		1 => array("pipe", "w"),
		2 => array("pipe", "w")
	);

	$ph = proc_open(
		"/bin/sleep 2",
		$descr,
		$pipes
	);
	$t = microtime(true);
	unset($ph);
	$t = microtime(true) - $t;
	if($t < 1) {
		echo "Fast enough.\n";
	} else {
		var_dump($t);
	}
?>
--EXPECT--
Fast enough.
