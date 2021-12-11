--TEST--
operator overload: inheritance for operators
--FILE--
<?php

interface Plus {

    operator +(int|float $other, OperandPosition $operandPos): mixed;

}

abstract class A {

    abstract operator +(int|float $other, OperandPosition $operandPos): self;

}

class B {
	public function test() {
		echo "test function".PHP_EOL;
	}

    public operator +(int|float $other, OperandPosition $operandPos): self
    {
    	echo "The + method was called.".PHP_EOL;
    	return $this;
    }
}

class C extends B implements Plus {}

$obj = new C();

$obj + 1;

?>
--EXPECT--
The + method was called.
