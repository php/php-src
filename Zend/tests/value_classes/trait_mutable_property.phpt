--TEST--
Value classes retain readonly trait compatibility checks
--FILE--
<?php
trait Mutable { public int $number; }
value class Foo { use Mutable; }
?>
--EXPECTF--
Fatal error: Readonly class Foo cannot use trait with a non-readonly property Mutable::$number in %s on line %d
