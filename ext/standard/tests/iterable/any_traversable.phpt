--TEST--
Test any() function
--FILE--
<?php

use function PHP\iterable\any;

/*
    Prototype: bool any(array $array, ?callable $callback = null, int $use_type = 0);
    Description: Iterate array and stop based on return value of callback
*/

function is_int_ex($item)
{
    return is_int($item);
}

function dump_any(...$args) {
    try {
        var_dump(any(...$args));
    } catch (Error $e) {
        printf("Caught %s: %s\n", $e::class, $e->getMessage());
    }
}


echo "\n*** Testing not enough or wrong arguments ***\n";

dump_any(new ArrayIterator());
dump_any(new ArrayIterator(), true);

echo "\n*** Testing basic functionality ***\n";

dump_any(new ArrayIterator([1, 2, 3]), 'is_int_ex');
dump_any(new ArrayIterator(['hello', 1, 2, 3]), 'is_int_ex');
$iterations = 0;
dump_any(new ArrayIterator(['hello', 1, 2, 3]), function($item) use (&$iterations) {
    ++$iterations;
    return is_int($item);
});
var_dump($iterations);

echo "\n*** Testing edge cases ***\n";

dump_any(new ArrayIterator([[], 0, '', 'hello']), function($item) {
    var_dump($item);
    return $item;
});

dump_any(new ArrayIterator(), 'is_int_ex');

dump_any(new ArrayObject(['key' => true]), null);

echo "testing generator\n";
function my_generator() {
    yield false;
    echo "Before returning []\n";
    yield [];
    echo "Before returning true\n";
    yield true;
    echo "Unreachable\n";
}
dump_any(my_generator());

function my_other_generator() {
    yield false;
    echo "Before my_other_generator() end\n";
}
dump_any(my_other_generator());

echo "\nDone";
?>
--EXPECT--
*** Testing not enough or wrong arguments ***
bool(false)
Caught TypeError: PHP\iterable\any(): Argument #2 ($callback) must be a valid callback, no array or string given

*** Testing basic functionality ***
bool(true)
bool(true)
bool(true)
int(2)

*** Testing edge cases ***
array(0) {
}
int(0)
string(0) ""
string(5) "hello"
bool(true)
bool(false)
bool(true)
testing generator
Before returning []
Before returning true
bool(true)
Before my_other_generator() end
bool(false)

Done