--TEST--
Test var_export() function : error conditions - recursive array
--FILE--
<?php
/* Description: Outputs or returns a string representation of a variable
 * Source code: ext/standard/var.c
 * Alias to functions:
 */

$a[] =& $a;
var_export($a, true);

?>
--EXPECTF--
Warning: var_export does not handle circular references in %s on line %d
