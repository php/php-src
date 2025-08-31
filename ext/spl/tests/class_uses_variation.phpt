--TEST--
SPL: Test class_uses() function : variation - no interfaces and autoload
--FILE--
<?php
echo "*** Testing class_uses() : variation ***\n";

echo "--- testing no traits ---\n";
class fs {}
var_dump(class_uses(new fs));
var_dump(class_uses('fs'));

spl_autoload_register(function ($classname) {
   echo "attempting to autoload $classname\n";
});

echo "\n--- testing autoload ---\n";
var_dump(class_uses('non_existent'));
var_dump(class_uses('non_existent2', false));

?>
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
