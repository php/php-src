--TEST--
Passing encoding as 3rd param to mb_strrpos (legacy)
--SKIPIF--
<?php if(!extension_loaded('mbstring')) die('skip mbstring not loaded'); ?>
--FILE--
<?php

mb_internal_encoding('UTF-16');
var_dump(mb_strrpos("abc abc abc", "abc", "UTF-8"));

?>
--EXPECTF--
Deprecated: mb_strrpos(): Passing the encoding as third parameter is deprecated. Use an explicit zero offset in %s on line %d
int(8)
