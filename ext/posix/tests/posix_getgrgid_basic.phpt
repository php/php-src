--TEST--
Test posix_getgrgid() function : basic functionality
--EXTENSIONS--
posix
--FILE--
<?php
  echo "Basic test of POSIX getgid and getgrid functions\n";

  $gid = posix_getgid();
  $groupinfo = posix_getgrgid($gid);

  print_r($groupinfo);

?>
--EXPECTF--
Basic test of POSIX getgid and getgrid functions
Array
(
    [name] => %s
    [passwd] => %A
    [members] => Array
%a

    [gid] => %d
)
