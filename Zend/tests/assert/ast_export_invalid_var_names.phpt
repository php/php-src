--TEST--
AST export of variable and property names that are not valid labels
--INI--
zend.assertions=1
--FILE--
<?php

$tests = [
    fn() => assert(false && ${'a b'}),
    fn() => assert(false && ${''}),
    fn() => assert(false && ${'1a'}),
    fn() => assert(false && ${"it's"}),
    fn() => assert(false && $o->{'a b'}),
    fn() => assert(false && $o?->{'a b'}),
    fn() => assert(false && $o->{'a b'}()),
    fn() => assert(false && $o?->{'a b'}()),
    fn() => assert(false && C::${'a b'}),
    fn() => assert(false && "x{${'a b'}}y"),
    fn() => assert(false && "x{$o->{'a b'}}y"),
    fn() => assert(false && ${'foo'}),
    fn() => assert(false && $o->{'foo'}),
];

foreach ($tests as $test) {
    try {
        $test();
    } catch (AssertionError $e) {
        echo $e::class, ': ', $e->getMessage(), "\n";
        $export = substr($e->getMessage(), strlen('assert('), -1);
        try {
            eval("return fn() => $export;");
        } catch (ParseError $e) {
            echo $e::class, ': ', $e->getMessage(), "\n";
        }
    }
}

?>
--EXPECT--
AssertionError: assert(false && ${'a b'})
AssertionError: assert(false && ${''})
AssertionError: assert(false && ${'1a'})
AssertionError: assert(false && ${'it\'s'})
AssertionError: assert(false && $o->{'a b'})
AssertionError: assert(false && $o?->{'a b'})
AssertionError: assert(false && $o->{'a b'}())
AssertionError: assert(false && $o?->{'a b'}())
AssertionError: assert(false && C::${'a b'})
AssertionError: assert(false && "x{${'a b'}}y")
AssertionError: assert(false && "x{$o->{'a b'}}y")
AssertionError: assert(false && $foo)
AssertionError: assert(false && $o->foo)
