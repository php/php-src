--TEST--
#[Attribute] on an abstract class
--FILE--
<?php

#[Attribute]
abstract class Demo {}

echo "Done\n";
?>
--EXPECT--
Done
