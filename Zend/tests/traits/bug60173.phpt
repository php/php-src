--TEST--
Bug #60173 (Wrong error message on reflective trait instantiation)
--FILE--
<?php

trait foo { }

$rc = new ReflectionClass('foo');
$rc->newInstance();

--EXPECTF--
Fatal error: Cannot instantiate trait foo in %s on line %d
