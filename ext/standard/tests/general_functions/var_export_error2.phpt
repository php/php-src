--TEST--
Test var_export() function : error conditions - recursive object
--FILE--
<?php
$obj = new stdClass;
$obj->p =& $obj;
var_export($obj, true);

?>
--EXPECTF--
Warning: var_export does not handle circular references in %s on line %d
