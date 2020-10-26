--TEST--
Test typed properties disallow incorrect type initial value (object)
--FILE--
<?php
class Foo {
    public stdClass $bar = null;
}
?>
--EXPECTF--
Fatal error: Property Foo:$bar of type stdClass cannot have a default value of null. Use the nullable type ?stdClass to allow a null default value in %s on line %d
