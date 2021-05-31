--TEST--
Test none() function
--FILE--
<?php

use function iterable\none;

/*
    Prototype: bool none(array $array, ?callable $callback = null, int $use_type = 0);
    Description: Iterate array and stop based on return value of callback
*/

function is_int_ex($item)
{
    return is_int($item);
}

function dump_none(...$args) {
    try {
        var_dump(none(...$args));
    } catch (Error $e) {
        printf("Caught %s: %s\n", $e::class, $e->getMessage());
    }
}


echo "\n*** Testing not enough or wrong arguments ***\n";

dump_none(new ArrayIterator());
dump_none(new ArrayIterator(), true);

echo "\n*** Testing basic functionality ***\n";

dump_none(new ArrayIterator([1, 2, 3]), 'is_int_ex');
dump_none(new ArrayIterator(['hello', 1, 2, 3]), 'is_int_ex');
$iterations = 0;
dump_none(new ArrayIterator(['hello', 1, 2, 3]), function($item) use (&$iterations) {
    ++$iterations;
    return is_int($item);
});
var_dump($iterations);

echo "\n*** Testing edge cases ***\n";

dump_none(new ArrayIterator([[], 0, '', 'hello']), function($item) {
    var_dump($item);
    return $item;
});

dump_none(new ArrayIterator(), 'is_int_ex');

dump_none(new ArrayObject(['key' => true]), null);

echo "testing generator\n";
function my_generator() {
    yield false;
    echo "Before returning []\n";
    yield [];
    echo "Before returning true\n";
    yield true;
    echo "Unreachable\n";
}
dump_none(my_generator());

function my_other_generator() {
    yield false;
    echo "Before my_other_generator() end\n";
}
dump_none(my_other_generator());

echo "\nDone";
?>
--EXPECT--
*** Testing not enough or wrong arguments ***
bool(true)
Caught TypeError: iterable\none(): Argument #2 ($callback) must be a valid callback or null, no array or string given

*** Testing basic functionality ***
bool(false)
bool(false)
bool(false)
int(2)

*** Testing edge cases ***
array(0) {
}
int(0)
string(0) ""
string(5) "hello"
bool(false)
bool(true)
bool(false)
testing generator
Before returning []
Before returning true
bool(false)
Before my_other_generator() end
bool(true)

Done
