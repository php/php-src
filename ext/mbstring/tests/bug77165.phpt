--TEST--
Bug #77165: mb_check_encoding crashes when argument given an empty array
--SKIPIF--
<?php require 'skipif.inc'; ?>
--FILE--
<?php
var_dump(mb_check_encoding(array()));
var_dump(mb_convert_encoding(array(), 'UTF-8'));
?>
--EXPECT--
bool(true)
array(0) {
}
