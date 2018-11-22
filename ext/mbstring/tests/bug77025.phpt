--TEST--
Bug #77025: mb_strpos throws Unknown encoding or conversion error
--SKIPIF--
<?php if(!extension_loaded('mbstring')) die('skip mbstring not loaded'); ?>
--FILE--
<?php

var_dump(mb_strpos('Hello', 'e', 0, '8bit'));
var_dump(mb_stripos('Hello', 'e', 0, '8bit'));

?>
--EXPECT--
int(1)
int(1)
