--TEST--
Test posix_getpgid() function : basic functionality 
--SKIPIF--
<?php 
	if (!extension_loaded('posix')) die('skip - POSIX extension not loaded'); 
?>
--FILE--
<?php 
  echo "Basic test of posix_getpgid function\n"; 
  
  $pid = posix_getpid();	
  $pgid = posix_getpgid($pid);
  
  var_dump($pgid); 
  
?>
===DONE====
--EXPECTF--
Basic test of posix_getpgid function
int(%d)
===DONE====

  