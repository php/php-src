--TEST--
Attribute flags type is not validated at compile time.
--FILE--
<?php

#[Attribute("foo")]
class A1 { }

?>
===DONE===
--EXPECT--
===DONE===
