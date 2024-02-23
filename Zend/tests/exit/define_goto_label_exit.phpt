--TEST--
Attempting to define a goto label called exit
--FILE--
<?php

echo "Before\n";

echo "In between\n";
exit:
echo "After\n";

?>
--EXPECT--
Before
In between
After
