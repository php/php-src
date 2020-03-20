--TEST--
Userspace operator overloading - Evaluation order
--FILE--
<?php
class A
{
    public static function __add($lhs, $rhs)
    {
        echo "__add on Class A called\n";
        return "A";
    }

    public static function __div($lhs, $rhs)
    {
        echo "__div on Class A called\n";
        return "A";
    }

    public static function __mul($lhs, $rhs)
    {
        echo "__mul on Class A called and skiped\n";
        return PHP_OPERAND_TYPES_NOT_SUPPORTED;
    }
}

class B
{
    public static function __add($lhs, $rhs)
    {
        echo "__add on Class B called\n";
        return "B";
    }

    public static function __mul($lhs, $rhs)
    {
        echo "__mul on Class B called\n";
        return "B";
    }
}

$a = new A();
$b = new B();

//Handler on left class is called
$a + $b;
$b + $a;

//Handler only existing in ClassA -> call ClassA
$a / $b;
$b / $a;

//Use PHP_OPERAND_TYPES_NOT_SUPPORTED to skip handler
$b * $a;
$a * $b;

--EXPECT--
__add on Class A called
__add on Class B called
__div on Class A called
__div on Class A called
__mul on Class B called
__mul on Class A called and skiped
__mul on Class B called
