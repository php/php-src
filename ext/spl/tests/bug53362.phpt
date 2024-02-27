--TEST--
Bug #53362 (Segmentation fault when extending SplFixedArray)
--FILE--
<?php

class obj extends SplFixedArray{
    public function offsetSet($offset, $value): void {
        var_dump($offset);
    }
}

$obj = new obj;

try {
    $obj[]=2;
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}
try {
    $obj[]=2;
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}
try {
    $obj[]=2;
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

?>
--EXPECT--
Error: Cannot append to object of type obj
Error: Cannot append to object of type obj
Error: Cannot append to object of type obj
