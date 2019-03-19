--TEST--
Bug #76127: preg_split does not raise an error on invalid UTF-8
--FILE--
<?php
var_dump(preg_split("/a/u", "a\xff"));
var_dump(preg_last_error() == PREG_BAD_UTF8_ERROR);
?>
--EXPECT--
bool(false)
bool(true)
