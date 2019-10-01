--TEST--
Trying assign value to property when an object is not returned in a function
--FILE--
<?php

class foo {
	public function a() {
	}
}

$test = new foo;

$test->a()->a;
print "ok\n";

try {
    $test->a()->a = 1;
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}
print "ok\n";

?>
--EXPECTF--
Warning: Trying to get property 'a' of non-object in %s on line %d
ok
Attempt to assign property 'a' of non-object
ok
