--TEST--
AST export of dynamic class constant fetches
--INI--
zend.assertions=1
--FILE--
<?php

$tests = [
    fn() => assert(false && C::FOO),
    fn() => assert(false && C::list),
    fn() => assert(false && C::class),
    fn() => assert(false && C::{$x}),
    fn() => assert(false && C::{FOO}),
    fn() => assert(false && C::{foo()}),
    fn() => assert(false && C::{$a . $b}),
    fn() => assert(false && C::{1}),
    fn() => assert(false && C::{'foo'}),
    fn() => assert(false && C::{'a b'}),
    fn() => assert(false && C::{''}),
    fn() => assert(false && C::{'class'}),
    fn() => assert(false && C::{'CLASS'}),
    fn() => assert(false && $o::{$x}),
    fn() => assert(false && static::{$x}),
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
AssertionError: assert(false && C::FOO)
AssertionError: assert(false && C::list)
AssertionError: assert(false && C::class)
AssertionError: assert(false && C::{$x})
AssertionError: assert(false && C::{FOO})
AssertionError: assert(false && C::{foo()})
AssertionError: assert(false && C::{$a . $b})
AssertionError: assert(false && C::{1})
AssertionError: assert(false && C::foo)
AssertionError: assert(false && C::{'a b'})
AssertionError: assert(false && C::{''})
AssertionError: assert(false && C::{'class'})
AssertionError: assert(false && C::{'CLASS'})
AssertionError: assert(false && $o::{$x})
AssertionError: assert(false && static::{$x})
