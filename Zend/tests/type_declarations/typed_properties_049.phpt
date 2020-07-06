--TEST--
Nullable typed property
--FILE--
<?php
class Foo {
    public int $foo = null;
}
?>
--EXPECTF--
Fatal error: Property Foo:$foo of type int cannot have a default value of null. Use the nullable type ?int to allow a null default value in %s on line %d
