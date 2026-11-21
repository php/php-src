--TEST--
GH-24250: protected(set)/private(set) not enforced after a write at the same call site was routed to __set()
--FILE--
<?php

class A {
    public protected(set) int $x = 1;
    public private(set) int $y = 1;
    public function __construct(bool $unset) {
        if ($unset) {
            unset($this->x, $this->y);
        }
    }
    public function __set($name, $value) {
        echo "__set($name)\n";
    }
}

function writeX(A $a, int $v) { $a->x = $v; }
function writeY(A $a, int $v) { $a->y = $v; }

writeX(new A(true), 1);
writeY(new A(true), 1);

$a = new A(false);
try {
    writeX($a, 42);
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}
try {
    writeY($a, 42);
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}
var_dump($a->x, $a->y);

class P {
    public private(set) int $x = 1;
    public private(set) readonly int $r;
    public function __construct(bool $unset) {
        if ($unset) {
            unset($this->x, $this->r);
        } else {
            $this->r = 1;
        }
    }
    public function __set($name, $value) {
        echo "__set($name)\n";
    }
}

class C extends P {
    public function writeX(int $v) { $this->x = $v; }
    public function __clone() { $this->r = 42; }
}

(new C(true))->writeX(1);
$c = new C(false);
try {
    $c->writeX(42);
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}
var_dump($c->x);

clone new C(true);
try {
    clone $c;
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}

?>
--EXPECT--
__set(x)
__set(y)
Cannot modify protected(set) property A::$x from global scope
Cannot modify private(set) property A::$y from global scope
int(1)
int(1)
__set(x)
Cannot modify private(set) property P::$x from scope C
int(1)
__set(r)
Cannot modify private(set) property P::$r from scope C
