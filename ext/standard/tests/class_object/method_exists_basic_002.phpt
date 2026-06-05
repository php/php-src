--TEST--
method_exists() on internal classes
--FILE--
<?php
echo " ---(Internal classes, using string class name)---\n";
echo "Does Exception::getMessage exist? ";
var_dump(method_exists("Exception", "getMessage"));
echo "Does stdClass::nonexistent exist? ";
var_dump(method_exists("stdClass", "nonexistent"));

echo "\n ---(Internal classes, using class instance)---\n";
echo "Does Exception::getMessage exist? ";
var_dump(method_exists(new Exception, "getMessage"));
echo "Does stdClass::nonexistent exist? ";
var_dump(method_exists(new stdClass, "nonexistent"));

echo "Done";
?>
--EXPECT--
 ---(Internal classes, using string class name)---
Does Exception::getMessage exist? bool(true)
Does stdClass::nonexistent exist? bool(false)

 ---(Internal classes, using class instance)---
Does Exception::getMessage exist? bool(true)
Does stdClass::nonexistent exist? bool(false)
Done
