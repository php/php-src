--TEST--
Test get_loaded_extensions() function : basic functionality
--FILE--
<?php
echo "*** Testing get_loaded_extensions() : basic functionality ***\n";

echo "Get loaded extensions\n";
var_dump(get_loaded_extensions());

?>
--EXPECTF--
*** Testing get_loaded_extensions() : basic functionality ***
Get loaded extensions
array(%d) {
%a
}
