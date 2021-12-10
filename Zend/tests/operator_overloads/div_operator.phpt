--TEST--
operator overload: div operator with scalars
--FILE--
<?php

class A {
    public $value;

    public operator /(int|float $other, OperandPosition $left): A
    {
    	$return = new A();

    	if ($left == OperandPosition::LeftSide) {
    		$return->value = $this->value / $other;
    	} else {
    		$return->value = $other / $this->value;
    	}

		return $return;
    }
}

$obj = new A();
$obj->value = 6;

$num1 = 12 / $obj;

var_dump($num1->value);

$num2 = $obj / 2;

var_dump($num2->value);
?>
--EXPECT--
int(2)
int(3)
