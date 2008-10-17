--TEST--
SPL: Test class_implements() function : variation - no interfaces and autoload
--FILE--
<?php
/* Prototype  : array class_implements(mixed what [, bool autoload ])
 * Description: Return all classes and interfaces implemented by SPL 
 * Source code: ext/spl/php_spl.c
 * Alias to functions: 
 */

echo "*** Testing class_implements() : variation ***\n";

echo "--- testing no interfaces ---\n";
class fs {}
var_dump(class_implements(new fs));
var_dump(class_implements('fs'));

echo "\n--- testing autoload ---\n";
var_dump(class_implements('non-existent'));
var_dump(class_implements('non-existent2', false));


function __autoload($classname) {
   echo "attempting to autoload $classname\n";
}

?>
===DONE===
--EXPECTF--
*** Testing class_implements() : variation ***
--- testing no interfaces ---
array(0) {
}
array(0) {
}

--- testing autoload ---
attempting to autoload non-existent

Warning: class_implements(): Class non-existent does not exist and could not be loaded in %s on line %d
bool(false)

Warning: class_implements(): Class non-existent2 does not exist in %s on line %d
bool(false)
===DONE===
