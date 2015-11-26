--TEST--
Test posix_getppid() function : basic functionality 
--SKIPIF--
<?php 
	if (!extension_loaded('posix')) die('skip - POSIX extension not loaded'); 
?>
--FILE--
<?php 
  echo "Basic test of POSIX getppid function\n"; 
  	
  $ppid = posix_getppid();
  
  var_dump($ppid); 
  
?>
===DONE====
--EXPECTF--
Basic test of POSIX getppid function
int(%d)
===DONE====
  