--TEST--
Test posix_getpwnam() function : basic functionality 
--CREDITS--
Rodrigo Prado de Jesus <royopa [at] gmail [dot] com>
User Group: PHPSP #phptestfestbrasil
--SKIPIF--
<?php 
	if (!extension_loaded('posix')) die('skip - POSIX extension not loaded'); 
?>
--FILE--
<?php 
  $uid = posix_geteuid();
  $user = posix_getpwuid($uid);
  print_r(posix_getpwnam($user['name']));
?>
===DONE====
--EXPECTREGEX--
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
