--TEST--
SPL: Test class_implements() function : basic
--FILE--
<?php
/* Prototype  : array class_uses(mixed what [, bool autoload ], [, string trait_name])
 * Description: Return all traits used by a class or check if a trait is used
 * Source code: ext/spl/php_spl.c
 * Alias to functions:
 */

echo "*** Testing class_uses() : basic ***\n";


trait foo { }
class bar { use foo; }

var_dump(class_uses(new bar, false, 'foo'));
var_dump(class_uses('bar', false, 'foo'));
var_dump(class_uses(new bar, false, 'bar'));
var_dump(class_uses(new bar, false, 'bat'));

?>
===DONE===
--EXPECT--
*** Testing class_uses() : basic ***
bool(true)
bool(true)
bool(false)
bool(false)
===DONE===
