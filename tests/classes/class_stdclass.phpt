--TEST--
Instantiate stdClass
--FILE--
<?php

$obj = new stdClass;

echo get_class($obj)."\n";

echo "Done\n";
?>
--EXPECT--
stdClass
Done
