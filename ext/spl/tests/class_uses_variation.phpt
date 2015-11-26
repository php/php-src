--TEST--
SPL: Test class_uses() function : variation - no interfaces and autoload
--FILE--
<?php
/* Prototype  : array class_uses(mixed what [, bool autoload ])
 * Description: Return all traits used by a class
 * Source code: ext/spl/php_spl.c
 * Alias to functions: 
 */

echo "*** Testing class_uses() : variation ***\n";

echo "--- testing no traits ---\n";
class fs {}
var_dump(class_uses(new fs));
var_dump(class_uses('fs'));

echo "\n--- testing autoload ---\n";
var_dump(class_uses('non_existent'));
var_dump(class_uses('non_existent2', false));


function __autoload($classname) {
   echo "attempting to autoload $classname\n";
}

?>
===DONE===
--EXPECTF--
*** Testing class_uses() : variation ***
--- testing no traits ---
array(0) {
}
array(0) {
}

--- testing autoload ---
attempting to autoload non_existent

Warning: class_uses(): Class non_existent does not exist and could not be loaded in %s on line %d
bool(false)

Warning: class_uses(): Class non_existent2 does not exist in %s on line %d
bool(false)
===DONE===
