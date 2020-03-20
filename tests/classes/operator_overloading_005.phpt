--TEST--
Userspace operator overloading - Error handling
--FILE--
<?php

class A
{
    public static function __add($lhs, $rhs)
    {
        return PHP_OPERAND_TYPES_NOT_SUPPORTED;
    }

    public static function __bitwiseNot($lhs)
    {
        return PHP_OPERAND_TYPES_NOT_SUPPORTED;
    }
}

$a = new A();

//Just trigger NOTICE when encounter an object without implemented handler
$a / 2;

try {
    $a + 1;
} catch(Error $exception) {
    echo $exception->getMessage() . "\n";
}

try {
    ~$a;
} catch(Error $exception) {
    echo $exception->getMessage() . "\n";
}

--EXPECTF--
Notice: You have to implement the __div function in class A to use this operator with an object in %s on line %d

Notice: Object of class A could not be converted to number in %s on line %d
The operand handlers __add do not support the given operand types combination (A and int given)
The operand handler __bitwiseNot do not support the given operand type (A given)
