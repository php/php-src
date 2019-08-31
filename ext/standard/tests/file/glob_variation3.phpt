--TEST--
Test glob() function: ensure no platform difference
--FILE--
<?php
$path = __DIR__;

ini_set('open_basedir', NULL);

var_dump(glob("$path/*.none"));
var_dump(glob("$path/?.none"));
var_dump(glob("$path/*{hello,world}.none"));
var_dump(glob("$path/*/nothere"));
var_dump(glob("$path/[aoeu]*.none"));
var_dump(glob("$path/directly_not_exists"));

var_dump(empty(ini_get('open_basedir')));
?>
==DONE==
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
==DONE==
