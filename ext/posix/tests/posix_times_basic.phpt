--TEST--
Test posix_times() function : basic functionality
--SKIPIF--
<?php
	if (!extension_loaded('posix')) die('skip - POSIX extension not loaded');
?>
--FILE--
<?php
  echo "Basic test of POSIX times function\n";

  $times = posix_times();

  var_dump($times);


  if ($times == FALSE) {
  	$errno= posix_get_last_error();
  	var_dump(posix_strerror($errno));
  }

?>
===DONE====
--EXPECTF--
Basic test of POSIX times function
array(5) {
  ["ticks"]=>
  int(%i)
  ["utime"]=>
  int(%i)
  ["stime"]=>
  int(%i)
  ["cutime"]=>
  int(%i)
  ["cstime"]=>
  int(%i)
}
===DONE====
