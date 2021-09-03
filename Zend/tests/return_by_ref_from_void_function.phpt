--TEST--
Returning by reference from void function is deprecated
--FILE--
<?php

function &test(): void {
}

$r =& test();
var_dump($r);

?>
--EXPECTF--
Deprecated: Returning by reference from a void function is deprecated in %s on line %d

Notice: Only variable references should be returned by reference in %s on line %d
NULL
