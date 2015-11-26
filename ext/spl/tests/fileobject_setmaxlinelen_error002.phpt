--TEST--
SPL: SplFileObject::setMaxLineLen error 002
--CREDITS--
Erwin Poeze <erwin.poeze at gmail.com>
--FILE--
<?php
$s = new SplFileObject( __FILE__ );
$s->setMaxLineLen();

?>
--EXPECTF--
Warning: SplFileObject::setMaxLineLen() expects exactly 1 parameter, 0 given in %s on line %d
