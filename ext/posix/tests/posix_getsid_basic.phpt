--TEST--
Test posix_getsid() function : basic functionality
--EXTENSIONS--
posix
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
  
