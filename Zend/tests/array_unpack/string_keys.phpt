--TEST--
array unpacking with string keys (not supported)
--FILE--
<?php

try {
    $array = [1, 2, "foo" => 3, 4];
    var_dump([...$array]);
} catch (Error $ex) {
    var_dump($ex->getMessage());
}
try {
    $iterator = new ArrayIterator([1, 2, "foo" => 3, 4]);
    var_dump([...$iterator]);
} catch (Error $ex) {
    var_dump($ex->getMessage());
}

?>
--EXPECT--
string(36) "Cannot unpack array with string keys"
string(42) "Cannot unpack Traversable with string keys"
