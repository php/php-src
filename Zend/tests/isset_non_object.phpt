--TEST--
isset() with object properties when operating on non-object
--FILE--
<?php

$foo = NULL;
isset($foo->bar->bar);

echo "Done\n";
?>
--EXPECT--
Done
