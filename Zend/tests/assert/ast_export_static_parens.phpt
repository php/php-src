--TEST--
assert() message keeps parentheses around the class operand of ::
--INI--
zend.assertions=1
--FILE--
<?php
class C {
    const X = false;
    public static $x = false;
    public static function m() { return false; }
}

$a = null;
$c = new C;

$tests = [
    fn() => assert(($a ?? $c)::$x),
    fn() => assert(($a ?? $c)::m()),
    fn() => assert(($a ?? $c)::X),
    fn() => assert(($a ?? $c)::class === 'D'),
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
AssertionError: assert(($a ?? $c)::$x)
AssertionError: assert(($a ?? $c)::m())
AssertionError: assert(($a ?? $c)::X)
AssertionError: assert(($a ?? $c)::class === 'D')
