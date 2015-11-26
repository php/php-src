--TEST--
SPL: SplFileObject::haschildren basic
--CREDITS--
Erwin Poeze <erwin.poeze at gmail.com>
--FILE--
<?php
$s = new SplFileObject( __FILE__ );
var_dump($s->hasChildren());

?>
--EXPECT--
bool(false)
