--TEST--
Test posix_getpgrp() function : basic functionality
--EXTENSIONS--
posix
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
  
