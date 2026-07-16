--TEST--
Literal types: valid variance (covariant returns, contravariant params)
--FILE--
<?php
class A {
    public function r(): int { return 1; }
    public function m(1|2 $x): void {}
}
class B extends A {
    public function r(): 1|2 { return 1; }
    public function m(1|2|3 $x): void {}
}

class C {
    public function r(): 1|2|3 { return 1; }
}
class D extends C {
    public function r(): 1|2 { return 1; }
}

echo "ok\n";

$b = new B();
var_dump($b->r());
$b->m(3);

$d = new D();
var_dump($d->r());
?>
--EXPECT--
ok
int(1)
int(1)
