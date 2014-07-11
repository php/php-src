--TEST--
SPL: SplFileObject::getchildren error 001
--CREDITS--
Erwin Poeze <erwin.poeze at gmail.com>
--FILE--
<?php
$s = new SplFileObject( __FILE__ );
$s->getChildren('string');

?>
--EXPECTF--
Warning: SplFileObject::getChildren() expects exactly 0 parameters, 1 given in %s on line %d
