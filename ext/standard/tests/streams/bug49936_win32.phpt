--TEST--
Bug #49936 (crash with ftp stream in php_stream_context_get_option())
--SKIPIF--
<?php
if( substr(PHP_OS, 0, 3) != "WIN" )
  die("skip. Do run on Windows only");
?>
--INI--
default_socket_timeout=2
--FILE--
<?php

$dir = 'ftp://your:self@localhost/';

var_dump(opendir($dir));
var_dump(opendir($dir));

?>
--EXPECTF--
Warning: opendir(): connect() failed: %s in %s on line %d

Warning: opendir(ftp://...@localhost/): Failed to open directory: operation failed in %s on line %d
bool(false)

Warning: opendir(): connect() failed: %s in %s on line %d

Warning: opendir(ftp://...@localhost/): Failed to open directory: operation failed in %s on line %d
bool(false)
