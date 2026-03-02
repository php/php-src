--TEST--
readonly property satisfies get only interface property
--FILE--
<?php
interface I {
    public int $prop { get; }
}
class C implements I {
    public function __construct(public readonly int $prop) {}
}
$c = new C(42);
var_dump($c->prop);
?>
--EXPECT--
int(42)
