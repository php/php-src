--TEST--
Bug #43353 wrong detection of 'data' wrapper
--SKIPIF--
<?php
if(substr(PHP_OS, 0, 3) == "WIN")
  die("skip on Windows");
?>
--INI--
allow_url_fopen=1
--FILE--
<?php

var_dump(is_dir('file:///datafoo:test'));
var_dump(is_dir('datafoo:test'));
var_dump(file_get_contents('data:text/plain,foo'));
var_dump(file_get_contents('datafoo:text/plain,foo'));

?>
--EXPECTF--
bool(false)
bool(false)
string(3) "foo"

Warning: file_get_contents(datafoo:text/plain,foo): failed to open stream: No such file or directory in %s
bool(false)
