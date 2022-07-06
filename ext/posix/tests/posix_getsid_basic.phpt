--TEST--
Test posix_getsid() function : basic functionality
--SKIPIF--
<?php
    if (!extension_loaded('posix')) die('skip - POSIX extension not loaded');
?>
--FILE--
<?php
  echo "Basic test of posix_getsid function\n";

  $pid = posix_getpid();
  $sid = posix_getsid($pid);

  var_dump($sid);

?>
===DONE====
--EXPECTF--
Basic test of posix_getsid function
int(%d)
===DONE====
  
