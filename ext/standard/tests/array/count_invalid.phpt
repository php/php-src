--TEST--
Only arrays and countable objects can be counted
--FILE--
<?php

try {
    $result = count(null);
    var_dump($result);
} catch (\TypeError $e) {
    echo $e->getMessage() . \PHP_EOL;
}

try {
    $result = count("string");
    var_dump($result);
} catch (\TypeError $e) {
    echo $e->getMessage() . \PHP_EOL;
}

try {
    $result = count(123);
    var_dump($result);
} catch (\TypeError $e) {
    echo $e->getMessage() . \PHP_EOL;
}

try {
    $result = count(true);
    var_dump($result);
} catch (\TypeError $e) {
    echo $e->getMessage() . \PHP_EOL;
}

try {
    $result = count(false);
    var_dump($result);
} catch (\TypeError $e) {
    echo $e->getMessage() . \PHP_EOL;
}

try {
    $result = count((object) []);
    var_dump($result);
} catch (\TypeError $e) {
    echo $e->getMessage() . \PHP_EOL;
}

?>
--EXPECT--
count(): Argument #1 ($value) must be of type Countable|array, null given
count(): Argument #1 ($value) must be of type Countable|array, string given
count(): Argument #1 ($value) must be of type Countable|array, int given
count(): Argument #1 ($value) must be of type Countable|array, bool given
count(): Argument #1 ($value) must be of type Countable|array, bool given
count(): Argument #1 ($value) must be of type Countable|array, stdClass given
