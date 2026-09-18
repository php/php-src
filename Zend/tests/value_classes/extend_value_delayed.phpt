--TEST--
Finality is checked when a value class is linked at runtime
--FILE--
<?php
if (true) { value class Foo {} }
class Bar extends Foo {}
?>
--EXPECTF--
Fatal error: Class Bar cannot extend final class Foo in %s on line %d
