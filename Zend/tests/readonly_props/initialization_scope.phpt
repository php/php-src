--TEST--
Initialization can only happen from private scope
--FILE--
<?php

class A {
    public readonly int $prop;

    public function initPrivate() {
        $this->prop = 3;
    }
}
class B extends A {
    public function initProtected() {
        $this->prop = 2;
    }
}

$test = new B;
try {
    $test->prop = 1;
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}
$test->initProtected();
var_dump($test);

$test = new B;
$test->initPrivate();
var_dump($test->prop);

// Rebinding bypass works.
$test = new B;
(function() {
    $this->prop = 1;
})->bindTo($test, A::class)();
var_dump($test->prop);

class C extends A {
    public readonly int $prop;
}

$test = new C;
$test->initPrivate();
var_dump($test->prop);

class X {
    public function initFromParent() {
        $this->prop = 1;
    }
}
class Y extends X {
    public readonly int $prop;
}

$test = new Y;
$test->initFromParent();
var_dump($test);

?>
--EXPECTF--
Cannot modify protected(set) readonly property A::$prop from global scope
object(B)#%d (1) {
  ["prop"]=>
  int(2)
}
int(3)
int(1)
int(3)
object(Y)#%d (1) {
  ["prop"]=>
  int(1)
}
