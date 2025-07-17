--TEST--
#[Attribute] on an enum
--FILE--
<?php

#[Attribute]
enum Demo {}

echo "Done\n";
?>
--EXPECT--
Done
