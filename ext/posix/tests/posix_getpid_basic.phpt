--TEST--
Test posix_getpid() function : basic functionality
--EXTENSIONS--
posix
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
  
