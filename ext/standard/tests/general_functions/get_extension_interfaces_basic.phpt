--TEST--
Test get_extension_interfaces() function: basic test
--FILE--
<?php
echo "Simple testcase for get_extension_interfaces() function\n";

$result = get_extension_interfaces("zend");
var_dump(gettype($result));
var_dump(in_array("Traversable", $result));

// Unknown extension
var_dump(get_extension_interfaces("foo"));

?>
--EXPECT--
Simple testcase for get_extension_interfaces() function
string(5) "array"
bool(true)
bool(false)
