--TEST--
Userspace operator overloading - Disallow typehint in arguments
--FILE--
<?php
class A
{
    public static function __add(int $lhs, $rhs)
    {
        return PHP_OPERAND_TYPES_NOT_SUPPORTED;
    }
}

$a = new A();
--EXPECTF--
Fatal error: Method A::__add() can not declare argument typehints in %s on line %d
