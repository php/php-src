--TEST--
Ensure a interface can not have private constants
--FILE--
<?php
interface A {
    private const FOO = 10;
}
--EXPECTF--
Fatal error: Interface constant A::FOO must have public visibility in %s on line %d
