--TEST--
operator overload: abstract
--FILE--
<?php

abstract class A {
    public int $value;

    abstract operator +(int $other, OperandPosition $left): self;
}

class B extends A {
    public operator +(int $other, OperandPosition $left): self
    {
        $return = new B();
        $return->value = $this->value + $other;

        return $return;
    }
}

$obj = new B();
$obj->value = 5;

echo ($obj + 2)->value.PHP_EOL;
?>
--EXPECT--
7
