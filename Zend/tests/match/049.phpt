--TEST--
Match expression error messages (zend.exception_ignore_args=1)
--INI--
zend.exception_ignore_args=1
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
UnhandledMatchError: Unhandled match case of type null
UnhandledMatchError: Unhandled match case of type int
UnhandledMatchError: Unhandled match case of type float
UnhandledMatchError: Unhandled match case of type float
UnhandledMatchError: Unhandled match case of type string
UnhandledMatchError: Unhandled match case of type bool
UnhandledMatchError: Unhandled match case of type bool
UnhandledMatchError: Unhandled match case of type array
UnhandledMatchError: Unhandled match case of type Beep
UnhandledMatchError: Unhandled match case of type string
UnhandledMatchError: Unhandled match case of type string
