--TEST--
__equals: Objects implementing compare_objects handler are unaffected
--SKIPIF--
<?php
if (!class_exists('DateTime')) {
    echo 'skip DateTime not available';
}
?>
--FILE--
<?php

date_default_timezone_set('America/Vancouver');

$a = new DateTime('2015-01-01'); // \
$b = new DateTime('2015-01-01'); // '- These two are equal according to compare_objects
$c = new DateTime('2015-01-02'); 

var_dump($a == $a); // true
var_dump($a == $b); // true
var_dump($a == $c); // false

var_dump($b == $a); // true
var_dump($b == $b); // true
var_dump($b == $c); // false

var_dump($c == $a); // false
var_dump($c == $b); // false
var_dump($c == $c); // true

?>
--EXPECT--
bool(true)
bool(true)
bool(false)
bool(true)
bool(true)
bool(false)
bool(false)
bool(false)
bool(true)

