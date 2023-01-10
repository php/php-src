--TEST--
Test get_extension_classes() function: basic test
--FILE--
<?php
echo "Simple testcase for get_extension_classes() function\n";

$result = get_extension_classes("zend");
var_dump(gettype($result));
var_dump(in_array("Exception", $result));

// Unknown extension
var_dump(get_extension_classes("foo"));

?>
--EXPECT--
Simple testcase for get_extension_classes() function
string(5) "array"
bool(true)
bool(false)
