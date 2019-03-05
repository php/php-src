--TEST--
Bug #20087 (Assertion failure)
--SKIPIF--
<?php extension_loaded('mbstring') or die('skip mbstring not available'); ?>
--FILE--
<?php
$testmoo = "blah blah";
var_dump(mb_parse_str("testmoo"));
var_dump($testmoo);
var_dump(mb_parse_str("test=moo"));
var_dump($test);
?>
--EXPECTF--
Deprecated: mb_parse_str(): Calling mb_parse_str() without the result argument is deprecated in %s on line %d
bool(true)
string(0) ""

Deprecated: mb_parse_str(): Calling mb_parse_str() without the result argument is deprecated in %s on line %d
bool(true)
string(3) "moo"
