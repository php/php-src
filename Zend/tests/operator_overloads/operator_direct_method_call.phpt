--TEST--
operator overload: disallowed method calls
--FILE--
<?php

class A {
	public function test() {
		echo "test function".PHP_EOL;
	}

    public operator +(int|float $other, OperandPosition $left): A
    {
    	echo "The + method was called.".PHP_EOL;
    	return $this;
    }
}

$obj = new A();
$op = '+';

$obj->test();

$obj->{'test'}();

try {
	$obj->{'+'}(1, OperandPosition::LeftSide);
} catch (Throwable $e) {
	echo $e->getMessage().PHP_EOL;
}

try {
	$obj->$op(1, OperandPosition::LeftSide);
} catch (Throwable $e) {
	echo $e->getMessage().PHP_EOL;
}

try {
	call_user_func([$obj, '+'], 1, OperandPosition::LeftSide);
} catch (Throwable $e) {
	echo $e->getMessage().PHP_EOL;
}

try {
	eval('$obj->{\'+\'}(1, OperandPosition::LeftSide);');
} catch (Throwable $e) {
	echo $e->getMessage().PHP_EOL;
}

try {
	$callable = [$obj, '+'];
	$callable(1, OperandPosition::LeftSide);
} catch (Throwable $e) {
	echo $e->getMessage().PHP_EOL;
}

try {
	call_user_func(['A', '+'], 1, OperandPosition::LeftSide);
} catch (Throwable $e) {
	echo $e->getMessage().PHP_EOL;
}

try {
	call_user_func('A::+', 1, OperandPosition::LeftSide);
} catch (Throwable $e) {
	echo $e->getMessage().PHP_EOL;
}
?>
--EXPECT--
test function
test function
Operator implementations may not be called as methods
Operator implementations may not be called as methods
Operator implementations may not be called as methods
Operator implementations may not be called as methods
Operator implementations may not be called as methods
call_user_func(): Argument #1 ($callback) must be a valid callback, non-static method A::+() cannot be called statically
call_user_func(): Argument #1 ($callback) must be a valid callback, non-static method A::+() cannot be called statically
