--TEST--
SPL: SplFileObject::haschildren basic
--CREDITS--
Erwin Poeze <erwin.poeze at gmail.com>
--INI--
include_path=.
--FILE--
<?php
$s = new SplFileObject( __FILE__ );
var_dump($s->hasChildren());

?>
--EXPECT--
bool(false)
