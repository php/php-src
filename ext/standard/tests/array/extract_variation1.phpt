--TEST--
Test extract() function (variation 1)
--FILE--
<?php

$val = 4;
$str = "John";

var_dump($val);
var_dump($str);

/* Extracting Global Variables */
var_dump(extract($GLOBALS, EXTR_REFS));
var_dump($val);
var_dump($str);

echo "\nDone";
?>
--EXPECTF--
int(4)
string(4) "John"
int(%d)
int(4)
string(4) "John"

Done
