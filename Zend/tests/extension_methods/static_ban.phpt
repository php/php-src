--TEST--
static:: is banned in extension methods
--FILE--
<?php
class C {}

extension C $c {
    public function m(): string { return static::class; }
}
?>
--EXPECTF--
Fatal error: Cannot use "static" in an extension method in %s on line %d