--TEST--
operator overload: static
--FILE--
<?php

class A {
    public int $value;

    static operator +(mixed $other, OperandPosition $left): self
    {
        $return = new A();
        $return->value = $this->value + $other;

        return $return;
    }
}
?>
--EXPECTF--
Fatal error: Operator %s::%s() cannot be static in %s on line %d
