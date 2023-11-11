--TEST--
array_rand() is not affected by MT_RAND_PHP as with PHP < 8.2
--FILE--
<?php

$array = [
    'foo',
    'bar',
    'baz',
];

mt_srand(1, MT_RAND_PHP);
function custom_array_rand() {
    global $array;
    $key = array_rand($array);
    var_dump('found key ' . $key);
    return $array[$key];
}

var_dump(
    custom_array_rand(),
    custom_array_rand(),
    custom_array_rand(),
);
?>
--EXPECTF--
string(11) "found key 0"
string(11) "found key 1"
string(11) "found key 0"
string(3) "foo"
string(3) "bar"
string(3) "foo"
