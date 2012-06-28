--TEST--
SPL: SplFileObject::haschildren error 001
--CREDITS--
Erwin Poeze <erwin.poeze at gmail.com>
--FILE--
<?php
$s = new SplFileObject( __FILE__ );
$s->hasChildren('string');

?>
--EXPECTF--
Warning: SplFileObject::hasChildren() expects exactly 0 parameters, 1 given in %s on line %d
