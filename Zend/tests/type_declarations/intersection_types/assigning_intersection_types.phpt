--TEST--
Assigning values to intersection types
--FILE--
<?php

interface X {}
interface Y {}
interface Z {}

class TestParent implements X, Y {}
class TestChild  extends TestParent implements Z {}

class A {

    public X&Y&Z $prop;

    public function method1(X&Y $a): X&Y&Z {
        return new TestChild();
    }
    public function method2(X $a): X&Y {
        return new TestParent();
    }
}

$tp = new TestParent();
$tc = new TestChild();

$o = new A();
try {
    $o->prop = $tp;
} catch (TypeError $e) {
    echo $e->getMessage(), \PHP_EOL;
}

$o->prop = $tc;

$r = $o->method1($tp);
var_dump($r);
$r = $o->method2($tp);
var_dump($r);
$r = $o->method1($tc);
var_dump($r);
$r = $o->method2($tc);
var_dump($r);


?>
--EXPECTF--
Cannot assign TestParent to property A::$prop of type X&Y&Z
object(TestChild)#%d (0) {
}
object(TestParent)#%d (0) {
}
object(TestChild)#%d (0) {
}
object(TestParent)#%d (0) {
}
