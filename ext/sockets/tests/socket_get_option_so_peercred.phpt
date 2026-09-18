--TEST--
socket_get_option() SO_PEERCRED
--EXTENSIONS--
sockets
posix
--SKIPIF--
<?php
if (PHP_OS_FAMILY !== 'Linux') die('skip Linux only');
?>
--FILE--
<?php
if (!socket_create_pair(AF_UNIX, SOCK_STREAM, 0, $pair)) {
    die('Unable to create AF_UNIX socket pair');
}

$cred = socket_get_option($pair[0], SOL_SOCKET, SO_PEERCRED);
var_dump($cred['pid'] === getmypid());
var_dump($cred['uid'] === posix_geteuid());
var_dump($cred['gid'] === posix_getegid());

socket_close($pair[0]);
socket_close($pair[1]);

$unconnected = socket_create(AF_UNIX, SOCK_STREAM, 0);
var_dump(socket_get_option($unconnected, SOL_SOCKET, SO_PEERCRED));
socket_close($unconnected);
?>
--EXPECT--
bool(true)
bool(true)
bool(true)
array(3) {
  ["pid"]=>
  int(0)
  ["uid"]=>
  int(-1)
  ["gid"]=>
  int(-1)
}
