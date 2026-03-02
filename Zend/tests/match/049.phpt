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
        print $e->getMessage() . PHP_EOL;
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
Unhandled match case of type null
Unhandled match case of type int
Unhandled match case of type float
Unhandled match case of type float
Unhandled match case of type string
Unhandled match case of type bool
Unhandled match case of type bool
Unhandled match case of type array
Unhandled match case of type Beep
Unhandled match case of type string
Unhandled match case of type string
