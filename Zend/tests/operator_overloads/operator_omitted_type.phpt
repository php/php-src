--TEST--
operator overload: no explicit type
--FILE--
<?php

class A {
    public int $value;

    public operator +($other, OperandPosition $left): self
    {
    	$return = new A();
    	$return->value = $this->value + $other;

		return $return;
    }
}
?>
--EXPECTF--
Fatal error: A::+(): Parameter #1 ($other) must explicitly define a type in %s on line %d
