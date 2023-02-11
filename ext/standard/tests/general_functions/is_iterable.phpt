--TEST--
Test is_iterable() function
--FILE--
<?php

function stdClassObjectFromJsonDecode() {
    $exampleKeyValueStore = [
        'key1' => 'value1',
        'key2' => 'value2',
        'key3' => 'value3'
    ];
    
    return json_decode(json_encode($exampleKeyValueStore));
}

function gen() {
    yield;
}

var_dump(is_iterable([1, 2, 3]));
var_dump(is_iterable(new ArrayIterator([1, 2, 3])));
var_dump(is_iterable(gen()));
var_dump(is_iterable(1));
var_dump(is_iterable(3.14));
var_dump(is_iterable(new stdClass()));
var_dump(is_iterable(stdClassObjectFromJsonDecode()));

?>
--EXPECT--
bool(true)
bool(true)
bool(true)
bool(false)
bool(false)
bool(true)
bool(true)
