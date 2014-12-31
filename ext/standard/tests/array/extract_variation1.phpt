--TEST--
Test extract() function (variation 1) 
--FILE--
<?php

$val = 4;
$str = "John";

debug_zval_dump($val);
debug_zval_dump($str);

/* Extracting Global Variables */
var_dump(extract($GLOBALS, EXTR_REFS));
debug_zval_dump($val);
debug_zval_dump($str);

echo "\nDone";
?>

--EXPECTF--
long(4)
string(4) "John" refcount(%d)
int(%d)
long(4)
string(4) "John" refcount(%d)

Done
