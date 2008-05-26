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

--EXPECT--
long(4) refcount(2)
unicode(4) "John" { 004a 006f 0068 006e } refcount(2)
int(10)
long(4) refcount(2)
unicode(4) "John" { 004a 006f 0068 006e } refcount(2)

Done
