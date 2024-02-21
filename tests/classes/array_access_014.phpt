--TEST--
Test ArrayAccess with array_key_exists()
--FILE--
<?php

class Test implements ArrayAccess
{
    public array $array = [];

    public function offsetExists($offset): bool {
        return array_key_exists($offset, $this->array);
    }

    public function offsetGet($offset): mixed {
        return $this->array[$offset];
    }

    public function offsetSet($offset, $data): void {
        $this->array[$offset] = $data;
    }

    public function offsetUnset($offset): void {
        unset($this->array[$offset]);
    }
}

$test = new Test();
$test[1] = "foo";

$func = "array_key_exists";
var_dump($func(0, $test));
var_dump($func(1, $test));

var_dump(array_key_exists(0, $test));
var_dump(array_key_exists(1, $test));

?>
--EXPECT--
bool(false)
bool(true)
bool(false)
bool(true)
