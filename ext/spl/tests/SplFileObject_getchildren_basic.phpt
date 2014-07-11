--TEST--
SPL: SplFileObject::getchildren basic
--CREDITS--
Erwin Poeze <erwin.poeze at gmail.com>
--FILE--
<?php
$s = new SplFileObject( __FILE__ );
var_dump($s->getChildren());

?>
--EXPECT--
NULL
