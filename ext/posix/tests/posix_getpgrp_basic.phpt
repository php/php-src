--TEST--
Test posix_getpgrp() function : basic functionality 
--SKIPIF--
<?php 
	if (!extension_loaded('posix')) die('skip - POSIX extension not loaded'); 
?>
--FILE--
<?php 
  echo "Basic test of POSIX getpgrp function\n"; 
  	
  $pgrp = posix_getpgrp();
  
  var_dump($pgrp); 
  
?>
===DONE====
--EXPECTF--
Basic test of POSIX getpgrp function
int(%d)
===DONE====
  