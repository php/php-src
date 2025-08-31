--TEST--
Ensure an interface cannot have protected constants
--FILE--
<?php
interface A {
    protected const FOO = 10;
}
?>
--EXPECTF--
Fatal error: Access type for interface constant A::FOO must be public in %s on line 3
