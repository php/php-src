--TEST--
Test posix_getgrgid() function : basic functionality
--SKIPIF--
<?php
	if (!extension_loaded('posix')) die('skip - POSIX extension not loaded');
?>
--FILE--
<?php
  echo "Basic test of POSIX getgid and getgrid fucntions\n";

  $gid = posix_getgid();
  $groupinfo = posix_getgrgid($gid);

  print_r($groupinfo);

?>
===DONE===
--EXPECTF--
Basic test of POSIX getgid and getgrid fucntions
Array
(
    [name] => %s
    [passwd] => %a
    [members] => Array
%a

    [gid] => %d
)
===DONE===
