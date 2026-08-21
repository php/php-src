--TEST--
Readonly property satisfies get-only abstract property
--FILE--
<?php
abstract class A {
    public abstract int $prop { get; }
}
class C extends A {
    public readonly int $prop;
    public function __construct(int $prop) {
        $this->prop = $prop;
    }
}
$c = new C(42);
var_dump($c->prop);
?>
--EXPECT--
int(42)
