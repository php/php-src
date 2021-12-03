--TEST--
operator overload: equals fallback to comparison operator
--FILE--
<?php

class A {
    public int $value;

    public operator <=>(mixed $other): int
    {
		return ($this->value <=> $other);
    }
}

$obj = new A();
$obj->value = 6;

$bool1 = 12 == $obj;

var_dump($bool1);

$bool2 = $obj == 2;

var_dump($bool2);

$bool3 = $obj == 6;

var_dump($bool3);

$bool4 = $obj == 6.0;

var_dump($bool4);

$bool5 = $obj != 6.0;

var_dump($bool5);
?>
--EXPECT--
bool(false)
bool(false)
bool(true)
bool(true)
bool(false)
