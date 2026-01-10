--TEST--
json_encode() Recursion test with JsonSerializable and serialize
--FILE--
<?php

class JsonEncodeFirstTest implements JsonSerializable
{
    public $a = 1;

    public function __serialize()
    {
        return [ 'result' => $this->a ];
    }

    public function jsonSerialize(): mixed
    {
        return [ 'serialize' => serialize($this) ];
    }
}

class SerializeFirstTest implements JsonSerializable
{
    public $a = 1;

    public function __serialize()
    {
        return [ 'result' => json_encode($this) ];
    }

    public function jsonSerialize(): mixed
    {
        return [ 'json' => serialize($this) ];
    }
}

var_dump(json_encode(new JsonEncodeFirstTest()));
var_dump(serialize(new SerializeFirstTest()));
?>
--EXPECT--
string(68) "{"serialize":"O:19:\"JsonEncodeFirstTest\":1:{s:6:\"result\";i:1;}"}"
string(113) "O:18:"SerializeFirstTest":1:{s:6:"result";s:62:"{"json":"O:18:\"SerializeFirstTest\":1:{s:6:\"result\";b:0;}"}";}"
