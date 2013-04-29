--TEST--
SPL: SplFileObject::setMaxLineLen error 003
--CREDITS--
Erwin Poeze <erwin.poeze at gmail.com>
--FILE--
<?php
$s = new SplFileObject( __FILE__ );
$s->setMaxLineLen('string');

?>
--EXPECTF--
Warning: SplFileObject::setMaxLineLen() expects parameter 1 to be long, string given in %s on line %d
