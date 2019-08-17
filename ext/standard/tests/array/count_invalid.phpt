--TEST--
Only arrays and countable objects can be counted
--FILE--
<?php

try {
    $result = count(null);
    var_dump($result);
} catch (\TypeError $e) {
    echo $e->getMessage() . "\n";
}

try {
    $result = count("string");
    var_dump($result);
} catch (\TypeError $e) {
    echo $e->getMessage() . "\n";
}

try {
    $result = count(123);
    var_dump($result);
} catch (\TypeError $e) {
    echo $e->getMessage() . "\n";
}

try {
    $result = count(true);
    var_dump($result);
} catch (\TypeError $e) {
    echo $e->getMessage() . "\n";
}
try {
    $result = count(false);
    var_dump($result);
} catch (\TypeError $e) {
    echo $e->getMessage() . "\n";
}
try {
    $result = count((object) []);
    var_dump($result);
} catch (\TypeError $e) {
    echo $e->getMessage() . "\n";
}

?>
--EXPECT--
Parameter must be an array or an object that implements Countable
Parameter must be an array or an object that implements Countable
Parameter must be an array or an object that implements Countable
Parameter must be an array or an object that implements Countable
Parameter must be an array or an object that implements Countable
Parameter must be an array or an object that implements Countable
