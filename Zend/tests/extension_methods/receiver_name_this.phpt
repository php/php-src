--TEST--
The receiver variable cannot be named $this
--FILE--
<?php
class C {}

extension C $this {
    public function m(): int { return 1; }
}
?>
--EXPECTF--
Fatal error: Cannot use $this as the extension receiver variable in %s on line %d