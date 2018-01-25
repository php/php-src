--TEST--
Ensure exceptions are handled properly when thrown in a statically declared __call.
--FILE--
<?php
class A {
	static function __call($strMethod, $arrArgs) {
		@var_dump($this);
		throw new Exception;
		echo "You should not see this";
	}
	function test() {
		A::unknownCalledWithSRO(1,2,3);
	}
}

class B extends A {
	function test() {
		B::unknownCalledWithSROFromChild(1,2,3);
	}
}

$a = new A();

echo "---> Invoke __call via simple method call.\n";
try {
	$a->unknown();
} catch (Exception $e) {
	echo "Exception caught OK; continuing.\n";
}

echo "\n\n---> Invoke __call via scope resolution operator within instance.\n";
try {
	$a->test();
} catch (Exception $e) {
	echo "Exception caught OK; continuing.\n";
}

echo "\n\n---> Invoke __call via scope resolution operator within child instance.\n";
$b = new B();
try {
	$b->test();
} catch (Exception $e) {
	echo "Exception caught OK; continuing.\n";
}

echo "\n\n---> Invoke __call via callback.\n";
try {
	call_user_func(array($b, 'unknownCallback'), 1,2,3);
} catch (Exception $e) {
	echo "Exception caught OK; continuing.\n";
}
?>
==DONE==
--EXPECTF--
Warning: The magic method __call() must have public visibility and cannot be static in %s on line 3
---> Invoke __call via simple method call.
object(A)#1 (0) {
}
Exception caught OK; continuing.


---> Invoke __call via scope resolution operator within instance.
object(A)#1 (0) {
}
Exception caught OK; continuing.


---> Invoke __call via scope resolution operator within child instance.
object(B)#2 (0) {
}
Exception caught OK; continuing.


---> Invoke __call via callback.
object(B)#2 (0) {
}
Exception caught OK; continuing.
==DONE==
