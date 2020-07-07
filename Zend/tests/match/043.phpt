--TEST--
Match expression error message
--FILE--
<?php

function test($value) {
    try {
        match ($value) {};
    } catch (UnhandledMatchError $e) {
        echo $e->getMessage() . "\n";
    }
}

test($a);
test([]);
test((object) []);
test(fopen('php://stdout', 'w'));
test(null);
test(false);
test(true);
test(42);
test(pi());
test("Foo bar");
test("Lorem ipsum dolor sit amet, consectetur adipiscing elit. Maecenas pretium ante tellus, a molestie sapien commodo sed. Nulla pellentesque nulla eget tellus fringilla tristique.");

?>
--EXPECTF--
Warning: Undefined variable $a in %s.php on line 11
Unhandled match value null
Unhandled match value of type array
Unhandled match value of type stdClass
Unhandled match value of type resource
Unhandled match value null
Unhandled match value false
Unhandled match value true
Unhandled match value 42
Unhandled match value 3.1415926535898
Unhandled match value "Foo bar"
Unhandled match value "Lorem ipsum dolor si..."
