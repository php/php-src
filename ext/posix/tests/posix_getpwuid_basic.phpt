--TEST--
Test posix_getpwuid() function : basic functionality
--SKIPIF--
<?php
    if (!extension_loaded('posix')) die('skip - POSIX extension not loaded');
?>
--FILE--
<?php
  echo "Basic test of POSIX getpwuid\n";


  $pwuid = posix_getpwuid(posix_getuid());

  print_r($pwuid);

?>
===DONE====
--EXPECTREGEX--
Basic test of POSIX getpwuid
Array
\(
    \[name\] => [^\r\n]+
    \[passwd\] => [^\r\n]+
    \[uid\] => [0-9]+
    \[gid\] => [0-9]+
    \[gecos\] => [^\r\n]*
    \[dir\] => [^\r\n]+
    \[shell\] => [^\r\n]+
\)
===DONE====
