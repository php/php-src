--TEST--
Ensure an interface cannot have private constants
--FILE--
<?php
interface A {
    private const FOO = 10;
}
?>
--EXPECTF--
Fatal error: Access type for interface constant A::FOO must be public in %s on line 3
