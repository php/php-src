--TEST--
A value class is implicitly final
--FILE--
<?php
value class Foo {}
class Bar extends Foo {}
?>
--EXPECTF--
Fatal error: Class Bar cannot extend final class Foo in %s on line %d
