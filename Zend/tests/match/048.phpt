--TEST--
Match expression error messages (exception_string_param_max_len=0)
--INI--
zend.exception_string_param_max_len=0
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
Unhandled match case NULL
Unhandled match case 1
Unhandled match case 5.5
Unhandled match case 5.0
Unhandled match case of type string
Unhandled match case true
Unhandled match case false
Unhandled match case of type array
Unhandled match case of type Beep
Unhandled match case of type string
Unhandled match case of type string
