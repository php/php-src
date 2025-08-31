--TEST--
json_encode() Recursion test with JsonSerializable, __debugInfo and print_r
--FILE--
<?php

class SerializingTest implements JsonSerializable
{
    public $a = 1;

    public function __debugInfo()
    {
        return [ 'result' => $this->a ];
    }

    public function jsonSerialize(): mixed
    {
        print_r($this);
        return $this;
    }
}

var_dump(json_encode(new SerializingTest()));
echo "---------\n";
var_dump(new SerializingTest());

?>
--EXPECT--
SerializingTest Object
(
    [result] => 1
)
string(7) "{"a":1}"
---------
object(SerializingTest)#1 (1) {
  ["result"]=>
  int(1)
}
