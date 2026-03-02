--TEST--
Test is_executable() function: error conditions
--FILE--
<?php
echo "\n*** Testing is_exceutable() on non-existent directory ***\n";
var_dump( is_executable(__DIR__."/is_executable") );

echo "Done\n";
?>
--EXPECT--
*** Testing is_exceutable() on non-existent directory ***
bool(false)
Done
