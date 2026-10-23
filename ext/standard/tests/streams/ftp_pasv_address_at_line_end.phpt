--TEST--
FTP wrapper must not read past the response buffer when the PASV address ends the line
--SKIPIF--
<?php
if (array_search('ftp',stream_get_wrappers()) === FALSE) die("skip ftp wrapper not available.");
if (!function_exists('pcntl_fork')) die("skip pcntl_fork() not available.");
?>
--INI--
default_socket_timeout=1
--FILE--
<?php

$pasv_address_at_line_end = true;
require __DIR__ . "/../../../ftp/tests/server.inc";

$path = "ftp://127.0.0.1:" . $port . "/bug73457";

var_dump(file_get_contents($path));
?>
--EXPECTF--
Warning: file_get_contents(): Failed to open stream: Failed to set up data channel: %s in %s on line %d
bool(false)
