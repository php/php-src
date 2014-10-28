--TEST--
Bug #34380: stream_socket_pair should set posix_globals.last_error on failure
--SKIPIF--
<?php
if (substr(PHP_OS, 0, 3) == "WIN")
	die("skip. Do not run on Windows");
if (!extension_loaded("posix"))
	die("skip. posix extension must be loaded");
?>
--FILE--
<?php

// invalid type to force EOPNOTSUPP (only STREAM_PF_UNIX is supported on *nix)
$sockets = stream_socket_pair(STREAM_PF_INET, STREAM_SOCK_STREAM, STREAM_IPPROTO_IP);
var_dump($sockets);
var_dump(posix_get_last_error());
?>
--EXPECTF--

Warning: stream_socket_pair(): failed to create sockets: [95]: Operation not supported in %s on line %d
bool(false)
int(95)
