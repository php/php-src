--TEST--
#[Attribute] on a trait
--FILE--
<?php

#[Attribute]
trait Demo {}

echo "Done\n";
?>
--EXPECT--
Done
