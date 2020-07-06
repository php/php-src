--TEST--
Ensure a interface can not have protected constants
--FILE--
<?php
interface A {
    protected const FOO = 10;
}
--EXPECTF--
Fatal error: Interface constant A::FOO must have public visibility in %s on line %d
