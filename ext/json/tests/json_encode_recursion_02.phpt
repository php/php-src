--TEST--
json_encode() Recursion test with JsonSerializable and var_dump simple
--FILE--
<?php

class SerializingTest implements JsonSerializable
{
    public $a = 1;

    public function jsonSerialize(): mixed
    {
        var_dump($this);
        return $this;
    }
}

var_dump(json_encode(new SerializingTest()));

?>
--EXPECT--
object(SerializingTest)#1 (1) {
  ["a"]=>
  int(1)
}
string(7) "{"a":1}"
