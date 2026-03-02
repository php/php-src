--TEST--
Test posix_getpwnam() function : basic functionality
--CREDITS--
Rodrigo Prado de Jesus <royopa [at] gmail [dot] com>
User Group: PHPSP #phptestfestbrasil
--EXTENSIONS--
posix
--FILE--
<?php
  $uid = posix_geteuid();
  $user = posix_getpwuid($uid);
  $username = $user['name'];
  $info = posix_getpwnam($username);
  print_r($info);
  var_dump($username == $info['name']);
?>
===DONE====
--EXPECTF--
Array
(
    [name] => %s
    [passwd] => %S
    [uid] => %d
    [gid] => %d
    [gecos] => %S
    [dir] => %s
    [shell] => %s
)
bool(true)
===DONE====
