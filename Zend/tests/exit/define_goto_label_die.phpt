--TEST--
Attempting to define a goto label called die
--FILE--
<?php

echo "Before\n";

echo "In between\n";
die:
echo "After\n";

?>
--EXPECT--
Before
In between
After
