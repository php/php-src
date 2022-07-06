--TEST--
SPL: Test class_implements() function : variation - no interfaces and autoload
--FILE--
<?php
echo "*** Testing class_implements() : variation ***\n";

echo "--- testing no interfaces ---\n";
class fs {}
var_dump(class_implements(new fs));
var_dump(class_implements('fs'));

spl_autoload_register(function ($classname) {
   echo "attempting to autoload $classname\n";
});

echo "\n--- testing autoload ---\n";
var_dump(class_implements('non_existent'));
var_dump(class_implements('non_existent2', false));

?>
--EXPECTF--
*** Testing class_implements() : variation ***
--- testing no interfaces ---
array(0) {
}
array(0) {
}

--- testing autoload ---
attempting to autoload non_existent

Warning: class_implements(): Class non_existent does not exist and could not be loaded in %s on line %d
bool(false)

Warning: class_implements(): Class non_existent2 does not exist in %s on line %d
bool(false)
