--TEST--
operator overload: unimplemented
--FILE--
<?php

class A {
    public int $value;
}

class B {
	public int $value;

	public operator +(int|A $other, OperandPosition $left): B
	{
		$return = new B();
		if (is_int($other)) {
			$return->value = $this->value + $other;
		} else {
			$return->value = $this->value + $other->value;
		}

		return $return;
	}
}

$obj1 = new A();
$obj1->value = 3;
$obj2 = new B();
$obj2->value = 2;

try {
	$num1 = $obj1 + 2;
} catch (InvalidOperatorError) {
	echo "OK!".PHP_EOL;
}

try {
	$num2 = 2 + $obj1;
} catch (InvalidOperatorError) {
	echo "STILL OK!".PHP_EOL;
}

$num3 = $obj1 + $obj2;

var_dump($num3->value);

$num4 = $obj2 + $obj1;

var_dump($num4->value);

?>
--EXPECT--
OK!
STILL OK!
int(5)
int(5)
