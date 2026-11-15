--TEST--
Anonymous class names are not truncated in runtime error messages
--FILE--
<?php

class A {
    public private(set) int $p = 0;
    private function f() {}
}

class B {
    private function __destruct() {}
}

$o = new class {
    public function __construct(public readonly int $ro = 1) {}
    private function priv() {}
    public function ret(): int { return "x"; }
    public function args($a) {}
    public function byRef(&$a) {}
    public function callPriv() { (new A)->f(); }
    public function setPrivateSet() { $a = new A; $a->p = 1; }
    public function destroy() { new B; }
};

$v = new class extends ArrayObject {
    public int $virtual { get => 1; }
};

foreach ([
    fn() => $o->priv(),
    fn() => $o->callPriv(),
    fn() => $o->setPrivateSet(),
    fn() => $o->destroy(),
    fn() => $o->ro = 2,
    fn() => $o->ret(),
    fn() => $o->args(),
    fn() => $v->__unserialize([0, [], ['virtual' => 2]]),
] as $fn) {
    try {
        $fn();
    } catch (Throwable $e) {
        echo $e::class, ': ', $e->getMessage(), "\n";
    }
}

$o->byRef(...new ArrayIterator([1]));

?>
--EXPECTF--
Error: Call to private method class@anonymous%0%s::priv() from global scope
Error: Call to private method A::f() from scope class@anonymous%0%s
Error: Cannot modify private(set) property A::$p from scope class@anonymous%0%s
Error: Call to private B::__destruct() from scope class@anonymous%0%s
Error: Cannot modify readonly property class@anonymous%0%s::$ro
TypeError: class@anonymous%0%s::ret(): Return value must be of type int, string returned
ArgumentCountError: Too few arguments to function class@anonymous%0%s::args(), 0 passed in %s on line %d and exactly 1 expected
Error: Cannot unserialize value for virtual property ArrayObject@anonymous%0%s::$virtual

Warning: Cannot pass by-reference argument 1 of class@anonymous%0%s::byRef() by unpacking a Traversable, passing by-value instead in %s on line %d
