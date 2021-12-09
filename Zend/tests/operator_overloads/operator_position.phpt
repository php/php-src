--TEST--
operator overload: overload called
--FILE--
<?php

class A {
    public operator +(mixed $other, OperandPosition $left): self
    {
    	if ($left == OperandPosition::RightSide) {
			echo "Right side".PHP_EOL;
		} elseif ($left == OperandPosition::LeftSide) {
			echo "Left side".PHP_EOL;
		} else {
    		echo "Unknown enum case!".PHP_EOL;
    	}
    	return $this;
    }
}

$obj = new A();

$obj + 1;
1 + $obj;
$obj++;
++$obj;

?>
--EXPECT--
Left side
Right side
Left side
Left side
