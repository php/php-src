--TEST--
Built-in value type names cannot be extension targets (see the scalar extension methods RFC)
--FILE--
<?php
extension string {
    public function length(): int { return strlen($this); }
}
?>
--EXPECTF--
Fatal error: Cannot extend reserved type string in %s on line %d