--TEST--
Nullable default property error message
--FILE--
<?php

class A {
    public string|int $b = null;
}

$t = new A;
$t->b;

?>
--EXPECTF--
Fatal error: Default value for property of type string|int may not be null. Use the nullable type string|int|null to allow null default value in %s on line %d
