--TEST--
operator overload: private visibility
--FILE--
<?php

class A {
    public int $value;

    private operator +(mixed $other, OperandPosition $left): self
    {
        $return = new A();
        $return->value = $this->value + $other;

        return $return;
    }
}
?>
--EXPECTF--
Fatal error: Operator %s::%s() must have public visibility in %s on line %d
