--TEST--
Reserved type names cannot be extension targets
--FILE--
<?php
extension iterable {
    public function m(): int { return 1; }
}
?>
--EXPECTF--
Fatal error: Cannot extend reserved type iterable in %s on line %d