--TEST--
Bug #63982 (isset() inconsistently produces a fatal error on protected property)
--FILE--
<?php
class Test {
        protected $protectedProperty;
}

$test = new Test();

var_dump(isset($test->protectedProperty));
var_dump(isset($test->protectedProperty->foo));
--EXPECTF--
bool(false)
bool(false)
