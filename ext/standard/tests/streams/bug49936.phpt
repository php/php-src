--TEST--
Bug #49936 (crash with ftp stream in php_stream_context_get_option())
--SKIPIF--
<?php
if( substr(PHP_OS, 0, 3) == "WIN" )
  die("skip. Do not run on Windows");
?>
--FILE--
<?php

$dir = 'ftp://your:self@localhost/';

var_dump(opendir($dir));
var_dump(opendir($dir));

?>
--EXPECTF--
Warning: opendir(ftp://...@localhost/): failed to open dir: not implemented in %s on line %d
bool(false)

Warning: opendir(ftp://...@localhost/): failed to open dir: not implemented in %s on line %d
bool(false)

