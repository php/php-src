--TEST--
__compareTo: Objects implementing compare_objects handler are unaffected
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

var_dump($a <=> $a); // 0
var_dump($a <=> $b); // 0
var_dump($a <=> $c); // -1

var_dump($b <=> $a); // 0
var_dump($b <=> $b); // 0
var_dump($b <=> $c); // -1

var_dump($c <=> $a); // 1
var_dump($c <=> $b); // 1
var_dump($c <=> $c); // 0

?>
--EXPECT--
int(0)
int(0)
int(-1)
int(0)
int(0)
int(-1)
int(1)
int(1)
int(0)
