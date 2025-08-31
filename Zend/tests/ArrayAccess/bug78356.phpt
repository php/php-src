--TEST--
Bug #78356: Array returned from ArrayAccess is incorrectly unpacked as argument
--FILE--
<?php
$object = new class implements ArrayAccess {
    public function offsetGet($offset): mixed
    {
        return [1, 2];
    }
    public function offsetExists($offset): bool
    {
        return true;
    }
    public function offsetUnset($offset): void {}
    public function offsetSet($offset, $value): void {}
};
var_dump(max(...$object[0]));
?>
--EXPECT--
int(2)
