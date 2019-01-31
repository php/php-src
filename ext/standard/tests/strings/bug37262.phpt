--TEST--
Bug #37262 (var_export() does not escape \0 character)
--FILE--
<?php
var_export("foo\0bar");
?>
--EXPECT--
'foo' . "\0" . 'bar'
