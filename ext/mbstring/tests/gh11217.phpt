--TEST--
GH-11217: Segfault in mb_strrpos/mb_strripos with ASCII encoding and negative offset
--EXTENSIONS--
mbstring
--FILE--
<?php
var_dump(mb_strrpos('foo', 'foo', -1, 'ASCII'));
var_dump(mb_strripos('foo', 'foo', -1, 'ASCII'));
?>
--EXPECT--
int(0)
int(0)
