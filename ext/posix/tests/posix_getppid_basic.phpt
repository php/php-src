--TEST--
Test posix_getppid() function : basic functionality
--EXTENSIONS--
posix
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
  
