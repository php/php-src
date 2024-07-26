--TEST--
Implementing offsetGet from DimensionReadable with ArrayAccess as returning by-ref must be allowed for BC
--FILE--
<?php

class Test implements DimensionReadable, ArrayAccess {
    public function &offsetGet(mixed $offset): mixed {}
    public function offsetExists(mixed $offset): bool {}
    public function offsetSet(mixed $offset, mixed $value): void {}
    public function offsetUnset(mixed $offset): void {}
}

?>
OK
--EXPECT--
OK
