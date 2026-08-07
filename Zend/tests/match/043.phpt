--TEST--
Match expression error messages
--FILE--
<?php

class Beep {}

function test(mixed $var) {
    try {
        match($var) {};
    } catch (UnhandledMatchError $e) {
        echo $e::class, ': ', $e->getMessage(), PHP_EOL;
    }
}

test(null);
test(1);
test(5.5);
test(5.0);
test("foo");
test(true);
test(false);
test([1, 2, 3]);
test(new Beep());
// Testing long strings.
test(str_repeat('e', 100));
test(str_repeat("e\n", 100));
?>
--EXPECT--
UnhandledMatchError: Unhandled match case NULL
UnhandledMatchError: Unhandled match case 1
UnhandledMatchError: Unhandled match case 5.5
UnhandledMatchError: Unhandled match case 5.0
UnhandledMatchError: Unhandled match case 'foo'
UnhandledMatchError: Unhandled match case true
UnhandledMatchError: Unhandled match case false
UnhandledMatchError: Unhandled match case of type array
UnhandledMatchError: Unhandled match case of type Beep
UnhandledMatchError: Unhandled match case 'eeeeeeeeeeeeeee...'
UnhandledMatchError: Unhandled match case 'e\ne\ne\ne\ne\ne\ne\ne...'
