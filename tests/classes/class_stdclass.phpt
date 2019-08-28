--TEST--
Instantiate StdClass
--FILE--
<?php

$obj = new StdClass;

echo get_class($obj)."\n";

echo "Done\n";
?>
--EXPECT--
StdClass
Done
