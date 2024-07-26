--TEST--
Implementing offsetGet from DimensionReadable as returning by-ref should error
--FILE--
<?php

class Test implements DimensionReadable {
    public function &offsetGet(mixed $offset): mixed {}
    public function offsetExists(mixed $offset): bool {}
}

?>
--EXPECTF--
Fatal error: DimensionReadable::offsetGet method must not return by reference, implement DimensionFetchable::offsetFetch instead in %s on line %d
