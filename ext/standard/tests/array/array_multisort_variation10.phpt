--TEST--
Test array_multisort() function : usage variation - testing with anonymous array arguments
--FILE--
<?php
echo "*** Testing array_multisort() : Testing with anonymous arguments ***\n";

var_dump(array_multisort(array(1,3,2,4)));

?>
--EXPECT--
*** Testing array_multisort() : Testing with anonymous arguments ***
bool(true)
