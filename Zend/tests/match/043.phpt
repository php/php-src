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
Unhandled match value: 1
Unhandled match value: 5.5
Unhandled match value: "foo"
Unhandled match value: true
Unhandled match value: false
Unhandled match value of type array
Unhandled match value of type Beep
Unhandled match value: "eeeeeeeeee..."
Unhandled match value: "e
e
e
e
e
..."
