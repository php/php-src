--TEST--
Optional interface with parent
--FILE--
<?php

interface I {}
class P implements I {}
class C extends P implements ?OptionalInterface {}

?>
===DONE===
--EXPECT--
===DONE===
