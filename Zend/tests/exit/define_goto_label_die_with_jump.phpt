--TEST--
Attempting to define a goto label called die and jump to it
--FILE--
<?php

echo "Before\n";
goto die;
echo "In between\n";
die:
echo "After\n";

?>
--EXPECT--
Before
After
