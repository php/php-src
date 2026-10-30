--TEST--
assert() message keeps parentheses around dereferenced and instanceof operands
--INI--
zend.assertions=1
--FILE--
<?php
class C {
    public $x = false;
    public function m() { return false; }
}
class D {}

$a = null;
$c = new C;

$tests = [
    fn() => assert(($a ?? $c)->x),
    fn() => assert(($a ?? $c)?->x),
    fn() => assert(($a ?? $c)->m()),
    fn() => assert(($a ?? $c)?->m()),
    fn() => assert(($a ?? $c) instanceof D),
    fn() => assert(-($c instanceof D)),
    fn() => assert(!$c instanceof C),
];

foreach ($tests as $test) {
    try {
        $test();
    } catch (AssertionError $e) {
        echo $e::class, ': ', $e->getMessage(), "\n";
    }
}
?>
--EXPECT--
AssertionError: assert(($a ?? $c)->x)
AssertionError: assert(($a ?? $c)?->x)
AssertionError: assert(($a ?? $c)->m())
AssertionError: assert(($a ?? $c)?->m())
AssertionError: assert(($a ?? $c) instanceof D)
AssertionError: assert(-($c instanceof D))
AssertionError: assert(!($c instanceof C))
