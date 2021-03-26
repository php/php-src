--TEST--
Arrow functions auto-capture variables, by value.
--FILE--
<?php

$a = 0;
$fn = fn() {
    var_dump(isset($a));
    $a = 1;
    var_dump($a);
};
var_dump($a);
$fn();
var_dump($a);

$a = 2;
$fn = fn() {
    $a++;
    var_dump($a);
};
var_dump($a);
$fn();
var_dump($a);

$a = 3;
$fn = fn() {
    if (false) $a = 4;
    var_dump($a);
};
var_dump($a);
$fn();
var_dump($a);

$a = 5;
$fn = fn() {
    var_dump($a);
    if (false) $a = 6;
};
var_dump($a);
$fn();
var_dump($a);

$a = 7;
$fn = fn() {
    unset($a);
    var_dump(isset($a));
};
var_dump($a);
$fn();
var_dump($a);

$a = 8;
$fn = fn() {
    $a = 9;
    unset($a);
    var_dump(isset($a));
};
var_dump($a);
$fn();
var_dump($a);

$a = 10;
$fn = fn() { return fn () { var_dump($a); }; };
var_dump($a);
$fn()();
var_dump($a);

?>
--EXPECT--
int(0)
bool(true)
int(1)
int(0)
int(2)
int(3)
int(2)
int(3)
int(3)
int(3)
int(5)
int(5)
int(5)
int(7)
bool(false)
int(7)
int(8)
bool(false)
int(8)
int(10)
int(10)
int(10)
