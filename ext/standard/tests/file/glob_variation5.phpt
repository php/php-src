--TEST--
Test glob() function: ensure no platform difference, variation 3
--SKIPIF--
<?php if( substr(PHP_OS, 0, 3) == "WIN" ) {die('skip not valid on Windows');} ?>
--FILE--
<?php
$path = dirname(__FILE__);

ini_set('open_basedir', '/tmp');

var_dump(glob("$path/*.none"));
var_dump(glob("$path/?.none"));
var_dump(glob("$path/*{hello,world}.none"));
var_dump(glob("$path/*/nothere"));
var_dump(glob("$path/[aoeu]*.none"));
var_dump(glob("$path/directly_not_exists"));

var_dump('/tmp' == ini_get('open_basedir'));
?>
==DONE==
--EXPECT--
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
bool(true)
==DONE==
