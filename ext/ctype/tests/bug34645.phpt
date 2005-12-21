--TEST--
Bug #34645 (ctype corrupts memory when validating large numbers)
--SKIPIF--
<?php if (!extension_loaded("ctype")) print "skip"; ?>
--FILE--
<?php
$id = 394829384;
var_dump(ctype_digit($id));
var_dump($id);
?>
--EXPECT--
bool(true)
int(394829384)
