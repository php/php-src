--TEST--
Inherited methods: parent's T-typed default value is re-checked against the substituted parameter type
--FILE--
<?php
class Box<T> {
    public function f(T $v = "fallback"): void { var_dump($v); }
    public function g(T $v = 0): void { var_dump($v); }
    public function h(?T $v = null): void { var_dump($v); }
}

class IntBox extends Box<int> {}

try {
    (new IntBox())->f();
} catch (TypeError $e) {
    echo "f: ", $e->getMessage(), "\n";
}

(new IntBox())->g();

(new IntBox())->h();

(new Box())->f();
(new Box())->g();
(new Box())->h();
?>
--EXPECTF--
f: Box::f(): Argument #1 ($v) must be of type int%S
int(0)
NULL
string(8) "fallback"
int(0)
NULL
