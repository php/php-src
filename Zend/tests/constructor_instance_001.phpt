--TEST--
Not allowed to call the constructor on an object instance
--FILE--
<?php
class C {}
$c = new C();
$c->__construct();
--EXPECTF--
Fatal error: Cannot call the constructor on an object instance %s
