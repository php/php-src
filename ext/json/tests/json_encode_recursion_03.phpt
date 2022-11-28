--TEST--
json_encode() Recursion test with JsonSerializable and __debugInfo
--FILE--
<?php

class SerializingTest implements JsonSerializable
{
    public $a = 1;

    public function __debugInfo()
    {
        return [ 'result' => json_encode($this) ];
    }

    public function jsonSerialize(): mixed
    {
        var_dump($this);
        return $this;
    }
}

var_dump(json_encode(new SerializingTest()));
echo "---------\n";
var_dump(new SerializingTest());

?>
--EXPECT--
*RECURSION*
object(SerializingTest)#1 (1) {
  ["result"]=>
  string(7) "{"a":1}"
}
string(7) "{"a":1}"
---------
*RECURSION*
object(SerializingTest)#1 (1) {
  ["result"]=>
  string(7) "{"a":1}"
}
