--TEST--
Parametric LSP: a non-generic class still uses standard LSP when the parent is non-generic
--FILE--
<?php
abstract class Base {
    public function f(int $x): int { return $x + 1; }
}

class Derived extends Base {
    public function f(int $x): int { return parent::f($x) * 2; }
}

echo (new Derived)->f(3), "\n";
?>
--EXPECT--
8
