--TEST--
Only arrays and countable objects can be counted
--FILE--
<?php

try {
    $result = count(null);
    var_dump($result);
} catch (\Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

try {
    $result = count("string");
    var_dump($result);
} catch (\Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

try {
    $result = count(123);
    var_dump($result);
} catch (\Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

try {
    $result = count(true);
    var_dump($result);
} catch (\Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

try {
    $result = count(false);
    var_dump($result);
} catch (\Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

try {
    $result = count((object) []);
    var_dump($result);
} catch (\Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

?>
--EXPECT--
TypeError: count(): Argument #1 ($value) must be of type Countable|array, null given
TypeError: count(): Argument #1 ($value) must be of type Countable|array, string given
TypeError: count(): Argument #1 ($value) must be of type Countable|array, int given
TypeError: count(): Argument #1 ($value) must be of type Countable|array, true given
TypeError: count(): Argument #1 ($value) must be of type Countable|array, false given
TypeError: count(): Argument #1 ($value) must be of type Countable|array, stdClass given
