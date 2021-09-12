--TEST--
Vector constructed from Traversable preserves keys
--FILE--
<?php

function yields_values() {
    yield 5 => (object)['key' => 'value'];
    yield 0 => new stdClass();;
    yield 0 => true;
}

function yields_bad_value() {
    yield 5 => (object)['key' => 'value'];
    yield -1 => new stdClass();
}

// Vector eagerly evaluates the passed in Traversable
$it = new Vector(yields_values());
foreach ($it as $key => $value) {
    printf("Key: %s\nValue: %s\n", var_export($key, true), var_export($value, true));
}
printf("count=%d capacity=%d\n", $it->count(), $it->capacity());
unset($it);

try {
    $it = new Vector(yields_bad_value());
} catch (Exception $e) {
    printf("Caught %s: %s\n", get_class($e), $e->getMessage());
}

?>
--EXPECT--
Key: 0
Value: true
Key: 1
Value: NULL
Key: 2
Value: NULL
Key: 3
Value: NULL
Key: 4
Value: NULL
Key: 5
Value: (object) array(
   'key' => 'value',
)
count=6 capacity=6
Caught UnexpectedValueException: array must contain only positive integer keys