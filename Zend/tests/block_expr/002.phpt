--TEST--
Test block expression in arrow functions
--FILE--
<?php

$a = 'a';

var_dump((fn() => {
    var_dump('b');
    var_dump('c');
    $a
})());

var_dump((fn() => {
    var_dump('d');
    var_dump('e');
    return 'f';
})());

var_dump((fn() => {
    var_dump('g');
    var_dump('h');
})());

var_dump((fn() => {})());

var_dump((fn() => {
    'i'
})());

?>
--EXPECT--
string(1) "b"
string(1) "c"
string(1) "a"
string(1) "d"
string(1) "e"
string(1) "f"
string(1) "g"
string(1) "h"
NULL
NULL
string(1) "i"
