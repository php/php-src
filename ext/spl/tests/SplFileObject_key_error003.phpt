--TEST--
SPL: SplFileObject::key error
--CREDITS--
Erwin Poeze <erwin.poeze AT gmail.com>
--FILE--
<?php
//line 2
//line 3
//line 4
//line 5
$s = new SplFileObject(__FILE__);
$s->key(3);
?>
--EXPECTF--
Warning: SplFileObject::key() expects exactly 0 parameters, 1 given in %s on line %d
