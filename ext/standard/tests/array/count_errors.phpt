--TEST--
Test count() function error conditions
--FILE--
<?php
/* Prototype: int count ( mixed $var [, int $mode] );
   Description: Count elements in an array, or properties in an object
*/

try {
    count(NULL, COUNT_NORMAL);
} catch (\TypeError $e) {
    echo $e->getMessage() . "\n";
}
try {
    count(NULL, COUNT_RECURSIVE);
} catch (\TypeError $e) {
    echo $e->getMessage() . "\n";
}
try {
    count(NULL);
} catch (\TypeError $e) {
    echo $e->getMessage() . "\n";
}

try {
    count("string", COUNT_NORMAL);
} catch (\TypeError $e) {
    echo $e->getMessage() . "\n";
}
try {
    count("string", COUNT_RECURSIVE);
} catch (\TypeError $e) {
    echo $e->getMessage() . "\n";
}
try {
    count("string");
} catch (\TypeError $e) {
    echo $e->getMessage() . "\n";
}
try {
    count("");
} catch (\TypeError $e) {
    echo $e->getMessage() . "\n";
}

try {
    count(100);
} catch (\TypeError $e) {
    echo $e->getMessage() . "\n";
}
try {
    count(-23.45);
} catch (\TypeError $e) {
    echo $e->getMessage() . "\n";
}

try {
    @count($a);
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
Parameter must be an array or an object that implements Countable
Parameter must be an array or an object that implements Countable
Parameter must be an array or an object that implements Countable
Parameter must be an array or an object that implements Countable
