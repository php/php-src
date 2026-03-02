--TEST--
Test glob() function: ensure no platform difference, variation 3
--SKIPIF--
<?php if( substr(PHP_OS, 0, 3) == "WIN" ) die('skip not valid directory on Windows'); ?>
--FILE--
<?php
$path = __DIR__;

$open_basedir = '/some_directory_we_are_hopefully_not_running_tests_from';
ini_set('open_basedir', $open_basedir);

var_dump(glob("$path/*.none"));
var_dump(glob("$path/?.none"));
var_dump(glob("$path/*{hello,world}.none"));
var_dump(glob("$path/*/nothere"));
var_dump(glob("$path/[aoeu]*.none"));
var_dump(glob("$path/directly_not_exists"));

var_dump($open_basedir == ini_get('open_basedir'));
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
