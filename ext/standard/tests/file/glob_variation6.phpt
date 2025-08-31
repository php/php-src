--TEST--
Test glob() function: ensure no platform difference, variation 4
--SKIPIF--
<?php if( substr(PHP_OS, 0, 3) != "WIN" ) die('skip only valid directory on Windows'); ?>
--FILE--
<?php
$path = __DIR__;

ini_set('open_basedir', 'c:\\windows');

var_dump(glob("$path/*.none"));
var_dump(glob("$path/?.none"));
var_dump(glob("$path/*{hello,world}.none"));
var_dump(glob("$path/*/nothere"));
var_dump(glob("$path/[aoeu]*.none"));
var_dump(glob("$path/directly_not_exists"));

var_dump('c:\\windows' == ini_get('open_basedir'));
?>
--EXPECT--
array(0) {
}
array(0) {
}
array(0) {
}
array(0) {
}
array(0) {
}
array(0) {
}
bool(true)
