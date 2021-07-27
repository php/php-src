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

?>
--EXPECT--
Unhandled match value: 1
Unhandled match value: 5.5
Unhandled match value: foo
Unhandled match value: true
Unhandled match value: false
Unhandled match value of type array
Unhandled match value of type Beep
