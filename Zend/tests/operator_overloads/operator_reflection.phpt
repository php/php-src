--TEST--
operator overload: reflection for operators
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

$reflector = new ReflectionObject($obj);

var_dump($reflector->hasMethod('test'));
var_dump($reflector->hasMethod('+'));
var_dump($reflector->hasOperator('test'));
var_dump($reflector->hasOperator('+'));

var_dump(count($reflector->getMethods()));
var_dump(count($reflector->getOperators()));

try {
	var_dump($reflector->getMethod('+'));
} catch (Throwable $e) {
	echo $e->getMessage().PHP_EOL;
}

try {
	var_dump($reflector->getOperator('test'));
} catch (Throwable $e) {
	echo $e->getMessage().PHP_EOL;
}

try {
	var_dump($reflector->getMethod('blank'));
} catch (Throwable $e) {
	echo $e->getMessage().PHP_EOL;
}

try {
	var_dump($reflector->getOperator('blank'));
} catch (Throwable $e) {
	echo $e->getMessage().PHP_EOL;
}

$operatorReflector = $reflector->getOperator('+');
$testReflector = $reflector->getMethod('test');

var_dump($operatorReflector instanceof ReflectionMethod);
var_dump($operatorReflector->isOperator());
var_dump($testReflector->isOperator());

$closure = $operatorReflector->getClosure($obj);

$closure(1, OperandPosition::LeftSide);

?>
--EXPECT--
bool(true)
bool(false)
bool(false)
bool(true)
int(1)
int(1)
Operator A::+() is not a method, use getOperator()
Operator A::test() does not exist
Method A::blank() does not exist
Operator A::blank() does not exist
bool(true)
bool(true)
bool(false)
The + method was called.
