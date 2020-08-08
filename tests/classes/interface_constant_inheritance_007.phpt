--TEST--
Ensure a interface can not have private constants
--FILE--
<?php
interface A {
    private const FOO = 10;
}
--EXPECTF--
Fatal error: Access type for interface constant A::FOO can not be private in %s on line 3
