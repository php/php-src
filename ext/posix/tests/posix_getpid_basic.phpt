--TEST--
Test posix_getpid() function : basic functionality 
--SKIPIF--
<?php 
	if (!extension_loaded('posix')) die('skip - POSIX extension not loaded'); 
?>
--FILE--
<?php 
  echo "Basic test of POSIX getpid function\n"; 
  	
  $pid = posix_getpid();
  
  var_dump($pid); 
  
?>
===DONE====
--EXPECTF--
Basic test of POSIX getpid function
int(%d)
===DONE====
  