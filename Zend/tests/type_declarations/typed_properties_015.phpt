--TEST--
Test typed properties disallow incorrect type initial value (object)
--FILE--
<?php
class Foo {
    public stdClass $bar = null;
}
?>
--EXPECTF--
Fatal error: Default value for property of type stdClass may not be null. Use the nullable type ?stdClass to allow null default value in %s on line %d
