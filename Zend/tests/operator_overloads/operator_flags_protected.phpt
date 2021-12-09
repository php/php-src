--TEST--
operator overload: protected visibility
--FILE--
<?php

class A {
    public int $value;

    protected operator +(mixed $other, OperandPosition $left): self
    {
        $return = new A();
        $return->value = $this->value + $other;

        return $return;
    }
}
?>
--EXPECTF--
Fatal error: Operator %s::%s() must have public visibility in %s on line %d
