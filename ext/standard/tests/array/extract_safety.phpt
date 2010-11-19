--TEST--
Test extract() for overwrite of GLOBALS
--FILE--
<?php
$str = "John";
debug_zval_dump($GLOBALS["str"]);

/* Extracting Global Variables */
$splat = array("foo" => "bar");
var_dump(extract(array("GLOBALS" => $splat, EXTR_OVERWRITE)));

unset ($splat);

debug_zval_dump($GLOBALS["str"]);

echo "\nDone";
?>

--EXPECTF--
string(4) "John" refcount(2)
int(0)
string(4) "John" refcount(2)

Done