--TEST--
Test array_multisort() function : usage variation - testing with empty array
--FILE--
<?php
echo "*** Testing array_multisort() : Testing with empty array ***\n";

var_dump(array_multisort(array()));

?>
--EXPECT--
*** Testing array_multisort() : Testing with empty array ***
bool(true)
