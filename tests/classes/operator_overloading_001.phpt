--TEST--
Userspace operator overloading - Handlers
--FILE--
<?php

class Test {
    public static function __add($lhs, $rhs)
    {
        echo "__add called\n";
        return 1;
    }

    public static function __sub($lhs, $rhs)
    {
        echo "__sub called\n";
        return 1;
    }

    public static function __mul($lhs, $rhs)
    {
        echo "__mul called\n";
        return 1;
    }

    public static function __div($lhs, $rhs)
    {
        echo "__div called\n";
        return 1;
    }

    public static function __pow($lhs, $rhs)
    {
        echo "__pow called\n";
        return 1;
    }

    public static function __mod($lhs, $rhs)
    {
        echo "__mod called\n";
        return 1;
    }

    public static function __concat($lhs, $rhs)
    {
        echo "__concat called\n";
        return 1;
    }

    public static function __shiftLeft($lhs, $rhs)
    {
        echo "__shiftLeft called\n";
        return 1;
    }

    public static function __shiftRight($lhs, $rhs)
    {
        echo "__shiftRight called\n";
        return 1;
    }

    public static function __bitwiseAnd($lhs, $rhs)
    {
        echo "__bitwiseAnd called\n";
        return 1;
    }

    public static function __bitwiseOr($lhs, $rhs)
    {
        echo "__bitwiseOr called\n";
        return 1;
    }

    public static function __bitwiseXor($lhs, $rhs)
    {
        echo "__bitwiseXor called\n";
        return 1;
    }

    public static function __bitwiseNot($lhs)
    {
        echo "__bitwiseNot called\n";
        return 1;
    }
}

$test = new Test();

//Test direct overloadable operators
$b = $test + 1;
$b = $test - 1;
$b = $test * 1;
$b = $test / 1;
$b = $test ** 1;
$b = $test % 1;
$b = $test . 1;
$b = $test << 1;
$b = $test >> 1;
$b = $test & 1;
$b = $test | 1;
$b = $test ^ 1;
$b = ~$test;

//Test indirect overloadable operators
$b = + $test;
$b = - $test;

$b = $test++;

//Increment overwrites $test, so recreate it
$test = new Test();
$b = $test--;

--EXPECT--
__add called
__sub called
__mul called
__div called
__pow called
__mod called
__concat called
__shiftLeft called
__shiftRight called
__bitwiseAnd called
__bitwiseOr called
__bitwiseXor called
__bitwiseNot called
__mul called
__mul called
__add called
__sub called
