--TEST--
Test var_export() function : error conditions - recursive array
--FILE--
<?php
$a[] =& $a;
var_export($a, true);

?>
--EXPECTF--
Warning: var_export does not handle circular references in %s on line %d
