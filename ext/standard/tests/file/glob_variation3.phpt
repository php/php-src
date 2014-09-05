--TEST--
Test glob() function: ensure no platform difference
--FILE--
<?php
$path = dirname(__FILE__);

ini_set('open_basedir', NULL);

var_dump(glob("$path/*.none"));
var_dump(glob("$path/?.none"));
var_dump(glob("$path/*{hello,world}.none"));
var_dump(glob("$path/*/nothere"));
var_dump(glob("$path/[aoeu]*.none"));
var_dump(glob("$path/directly_not_exists"));

$b = ini_get('open_basedir');
var_dump(empty($b));
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
