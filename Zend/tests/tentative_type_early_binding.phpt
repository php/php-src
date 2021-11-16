--TEST--
Check that both warning and unresolved during early binding is handled
--FILE--
<?php
class Test extends SplObjectStorage {
    function valid() {}
    function current(): Unknown {}
}
?>
--EXPECTF--
Deprecated: Return type of Test::valid() should either be compatible with SplObjectStorage::valid(): bool, or the #[\ReturnTypeWillChange] attribute should be used to temporarily suppress the notice in %s on line %d

Fatal error: Could not check compatibility between Test::current(): Unknown and SplObjectStorage::current(): object, because class Unknown is not available in %s on line %d
