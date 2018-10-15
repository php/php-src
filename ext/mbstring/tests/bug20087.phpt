--TEST--
Bug #20087 (Assertion failure)
--SKIPIF--
<?php extension_loaded('mbstring') or die('skip mbstring not available'); ?>
--FILE--
<?php
ini_set('include_path', dirname(__FILE__));
include_once('common.inc');
$testmoo = "blah blah";
var_dump(mb_parse_str("testmoo"));
var_dump($testmoo);
var_dump(mb_parse_str("test=moo"));
var_dump($test);
?>
--EXPECT--
ERR: Deprecated
bool(true)
string(0) ""
ERR: Deprecated
bool(true)
string(3) "moo"
