--TEST--
Union of null and intersection type
--FILE--
<?php

interface X {}
interface Y {}

class A implements X, Y {}
class C {}

class Test {
    public (X&Y)|null $prop1;
    public null|(X&Y) $prop2;

    public function foo1((X&Y)|null $v): (X&Y)|null {
        var_dump($v);
        return $v;
    }
    public function foo2(null|(X&Y) $v): null|(X&Y) {
        var_dump($v);
        return $v;
    }
}

$test = new Test();
$a = new A();
$n = null;

$test->foo1($a);
$test->foo2($a);
$test->foo1($n);
$test->foo2($n);
$test->prop1 = $a;
$test->prop1 = $n;
$test->prop2 = $a;
$test->prop2 = $n;

$c = new C();
try {
    $test->foo1($c);
} catch (\TypeError $e) {
    echo $e->getMessage(), \PHP_EOL;
}
try {
    $test->foo2($c);
} catch (\TypeError $e) {
    echo $e->getMessage(), \PHP_EOL;
}
try {
    $test->prop1 = $c;
} catch (\TypeError $e) {
    echo $e->getMessage(), \PHP_EOL;
}
try {
    $test->prop2 = $c;
} catch (\TypeError $e) {
    echo $e->getMessage(), \PHP_EOL;
}

?>
===DONE===
--EXPECTF--
object(A)#2 (0) {
}
object(A)#2 (0) {
}
NULL
NULL
Test::foo1(): Argument #1 ($v) must be of type (X&Y)|null, C given, called in %s on line %d
Test::foo2(): Argument #1 ($v) must be of type (X&Y)|null, C given, called in %s on line %d
Cannot assign C to property Test::$prop1 of type (X&Y)|null
Cannot assign C to property Test::$prop2 of type (X&Y)|null
===DONE===
