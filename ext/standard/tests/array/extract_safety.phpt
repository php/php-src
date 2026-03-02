--TEST--
Test extract() for overwrite of GLOBALS
--FILE--
<?php
$str = "John";
var_dump($GLOBALS["str"]);

/* Extracting Global Variables */
$splat = array("foo" => "bar");
var_dump(extract(array("GLOBALS" => $splat, EXTR_OVERWRITE)));

unset ($splat);

var_dump($GLOBALS["str"]);

echo "\nDone";
?>
--EXPECT--
string(4) "John"
int(1)
string(4) "John"

Done
