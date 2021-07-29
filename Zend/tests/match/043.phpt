--TEST--
Match expression error messages
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

test(1);
test(5.5);
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
Unhandled match 1
Unhandled match 5.5
Unhandled match 'foo'
Unhandled match true
Unhandled match false
Unhandled match of type array
Unhandled match of type Beep
Unhandled match 'eeeeeeeeeeeeeee...'
Unhandled match 'e\ne\ne\ne\ne\ne\ne\ne...'
