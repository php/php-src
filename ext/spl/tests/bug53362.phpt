--TEST--
Bug #53362 (Segmentation fault when extending SplFixedArray)
--FILE--
<?php

class obj extends SplFixedArray{
    public function offsetSet($offset, $value) {
        var_dump($offset);
    }
}

$obj = new obj;

try {
    $obj[]=2;
} catch (\Error $e) {
    echo $e->getMessage() . \PHP_EOL;
}
try {
    $obj[]=2;
} catch (\Error $e) {
    echo $e->getMessage() . \PHP_EOL;
}
try {
    $obj[]=2;
} catch (\Error $e) {
    echo $e->getMessage() . \PHP_EOL;
}

?>
--EXPECT--
Dynamic allocation is forbidden
Dynamic allocation is forbidden
Dynamic allocation is forbidden
