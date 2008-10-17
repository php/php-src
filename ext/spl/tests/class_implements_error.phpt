--TEST--
SPL: Test class_implements() function : error 
--FILE--
<?php
/* Prototype  : array class_implements(mixed what [, bool autoload ])
 * Description: Return all classes and interfaces implemented by SPL 
 * Source code: ext/spl/php_spl.c
 * Alias to functions: 
 */

echo "*** Testing class_implements() : error ***\n";

interface foo { }
class bar implements foo {}

var_dump(class_implements());
var_dump(class_implements("bar", true, 10));
?>
===DONE===
--EXPECTF--
*** Testing class_implements() : error ***

Warning: class_implements() expects at least 1 parameter, 0 given in %s on line %d
bool(false)

Warning: class_implements() expects at most 2 parameters, 3 given in %s on line %d
bool(false)
===DONE===
