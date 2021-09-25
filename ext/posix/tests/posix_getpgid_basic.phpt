--TEST--
Test posix_getpgid() function : basic functionality
--EXTENSIONS--
posix
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

  
