--TEST--
Bug #64106: Segfault on SplFixedArray[][x] = y when extended
--FILE--
<?php

class MyFixedArray extends SplFixedArray {
    public function offsetGet($offset): mixed { var_dump($offset);  return null; }
}

$array = new MyFixedArray(10);
try {
    $array[][1] = 10;
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

?>
--EXPECT--
Error: Cannot fetch append object of type MyFixedArray
