--TEST--
Union of a simple and intersection type
--FILE--
<?php

interface X {}
interface Y {}

class A implements X, Y {}
class B {}
class C {}

class Test {
    public (X&Y)|int $prop1;
    public int|(X&Y) $prop2;
    public (X&Y)|B $prop3;
    public B|(X&Y) $prop4;

    public function foo1((X&Y)|int $v): (X&Y)|int {
        var_dump($v);
        return $v;
    }
    public function foo2(int|(X&Y) $v): int|(X&Y) {
        var_dump($v);
        return $v;
    }
    public function bar1(B|(X&Y) $v): B|(X&Y) {
        var_dump($v);
        return $v;
    }
    public function bar2((X&Y)|B $v): (X&Y)|B {
        var_dump($v);
        return $v;
    }
}

$test = new Test();
$a = new A();
$b = new B();
$i = 10;

$test->foo1($a);
$test->foo2($a);
$test->foo1($i);
$test->foo2($i);
$test->prop1 = $a;
$test->prop1 = $i;
$test->prop2 = $a;
$test->prop2 = $i;

$test->bar1($a);
$test->bar2($a);
$test->bar1($b);
$test->bar2($b);
$test->prop3 = $a;
$test->prop4 = $b;
$test->prop3 = $a;
$test->prop4 = $b;

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
    $test->bar1($c);
} catch (\TypeError $e) {
    echo $e->getMessage(), \PHP_EOL;
}
try {
    $test->bar2($c);
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
try {
    $test->prop3 = $c;
} catch (\TypeError $e) {
    echo $e->getMessage(), \PHP_EOL;
}
try {
    $test->prop4 = $c;
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
int(10)
int(10)
object(A)#2 (0) {
}
object(A)#2 (0) {
}
object(B)#3 (0) {
}
object(B)#3 (0) {
}
Test::foo1(): Argument #1 ($v) must be of type (X&Y)|int, C given, called in %s on line %d
Test::foo2(): Argument #1 ($v) must be of type (X&Y)|int, C given, called in %s on line %d
Test::bar1(): Argument #1 ($v) must be of type B|(X&Y), C given, called in %s on line %d
Test::bar2(): Argument #1 ($v) must be of type (X&Y)|B, C given, called in %s on line %d
Cannot assign C to property Test::$prop1 of type (X&Y)|int
Cannot assign C to property Test::$prop2 of type (X&Y)|int
Cannot assign C to property Test::$prop3 of type (X&Y)|B
Cannot assign C to property Test::$prop4 of type B|(X&Y)
===DONE===
