--TEST--
Test var_export() function : error conditions - recursive array
--FILE--
<?php
/* Prototype  : mixed var_export(mixed var [, bool return])
 * Description: Outputs or returns a string representation of a variable 
 * Source code: ext/standard/var.c
 * Alias to functions: 
 */

$a[] =& $a;
var_export($a, true);

?>
===DONE===
--EXPECTF--

Fatal error: Nesting level too deep - recursive dependency? in %s on line 9