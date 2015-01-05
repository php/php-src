--TEST--
SPL: SplFileObject::getMaxLineLen error 001
--CREDITS--
Erwin Poeze <erwin.poeze at gmail.com>
--INI--
include_path=.
--FILE--
<?php
$s = new SplFileObject( __FILE__ );
$s->getMaxLineLen('string');

?>
--EXPECTF--
Warning: SplFileObject::getMaxLineLen() expects exactly 0 parameters, 1 given in %s on line %d
