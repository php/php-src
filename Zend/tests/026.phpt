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
Warning: Attempt to read property "a" on null in %s on line %d
ok
Attempt to assign property "a" on null
ok
