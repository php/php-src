--TEST--
AST export of names containing 0x7f
--INI--
zend.assertions=1
--FILE--
<?php

$tests = [
    fn() => assert(false && C::{"\x7f"}),
    fn() => assert(false && C::{"a\x7f"}),
    fn() => assert(false && C::{"\x7fa"}),
    fn() => assert(false && $o::{"\x7f"}),
    fn() => assert(false && C::{"\x7f"}()),
    fn() => assert(false && $o::{"\x7f"}()),
    fn() => assert(false && ${"\x7f"}),
    fn() => assert(false && ${"a\x7f"}),
    fn() => assert(false && $o->{"\x7f"}),
    fn() => assert(false && $o?->{"\x7f"}),
    fn() => assert(false && $o->{"\x7f"}()),
    fn() => assert(false && $o?->{"\x7f"}()),
    fn() => assert(false && C::${"\x7f"}),
    fn() => assert(false && "x{${"\x7f"}}y"),
    fn() => assert(false && "x{$o->{"\x7f"}}y"),
    fn() => assert(false && "{$a}\x7f"),
    fn() => assert(false && C::{"\x80"}),
    fn() => assert(false && ${"\xff"}),
    fn() => assert(false && $o->{"a\x80"}),
    fn() => assert(false && "{$a}\x80"),
];

foreach ($tests as $test) {
    try {
        $test();
    } catch (AssertionError $e) {
        echo $e::class, ': ', addcslashes($e->getMessage(), "\x7f..\xff"), "\n";
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
AssertionError: assert(false && C::{'\177'})
AssertionError: assert(false && C::{'a\177'})
AssertionError: assert(false && C::{'\177a'})
AssertionError: assert(false && $o::{'\177'})
AssertionError: assert(false && C::{'\177'}())
AssertionError: assert(false && $o::{'\177'}())
AssertionError: assert(false && ${'\177'})
AssertionError: assert(false && ${'a\177'})
AssertionError: assert(false && $o->{'\177'})
AssertionError: assert(false && $o?->{'\177'})
AssertionError: assert(false && $o->{'\177'}())
AssertionError: assert(false && $o?->{'\177'}())
AssertionError: assert(false && C::${'\177'})
AssertionError: assert(false && "x{${'\177'}}y")
AssertionError: assert(false && "x{$o->{'\177'}}y")
AssertionError: assert(false && "$a\177")
AssertionError: assert(false && C::\200)
AssertionError: assert(false && $\377)
AssertionError: assert(false && $o->a\200)
AssertionError: assert(false && "{$a}\200")
