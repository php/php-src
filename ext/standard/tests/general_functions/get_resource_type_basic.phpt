--TEST--
Test get_resource_type() function : basic functionality
--FILE--
<?php
echo "*** Testing get_resource_type() : basic functionality ***\n";

$res = fopen(__FILE__, "r");
var_dump(get_resource_type($res));

?>
--EXPECT--
*** Testing get_resource_type() : basic functionality ***
string(6) "stream"
