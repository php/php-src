--TEST--
operator overload: sub operator with scalars
--FILE--
<?php

class A {
    public int $value;

    public operator -(int $other, OperandPosition $left): A
    {
    	$return = new A();

    	if ($left == OperandPosition::LeftSide) {
    		$return->value = $this->value - $other;
    	} else {
    		$return->value = $other - $this->value;
    	}

		return $return;
    }
}

$obj = new A();
$obj->value = 3;

$num1 = 2 - $obj;

var_dump($num1->value);

$num2 = $obj - 3;

var_dump($num2->value);
?>
--EXPECT--
int(-1)
int(0)
