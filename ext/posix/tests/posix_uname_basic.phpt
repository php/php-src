--TEST--
Test posix_uname() function : basic functionality
--EXTENSIONS--
posix
--FILE--
<?php
  echo "Basic test of POSIX uname function\n";

  $uname = posix_uname();
  unset($uname['domainname']);
  print_r($uname);

?>
===DONE====
--EXPECTF--
Basic test of POSIX uname function
Array
(
    [sysname] => %s
    [nodename] => %s
    [release] => %s
    [version] => %s
    [machine] => %s
)
===DONE====
  
