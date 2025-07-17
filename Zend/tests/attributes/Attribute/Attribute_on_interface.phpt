--TEST--
#[Attribute] on an interface
--FILE--
<?php

#[Attribute]
interface Demo {}

echo "Done\n";
?>
--EXPECT--
Done
