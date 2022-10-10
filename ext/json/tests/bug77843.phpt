--TEST--
Bug #77843: Use after free with json serializer
--FILE--
<?php

class X implements JsonSerializable {
    public $prop = "value";
    public function jsonSerialize(): mixed {
        global $arr;
        unset($arr[0]);
        var_dump($this);
        return $this;
    }
}

$arr = [new X()];
var_dump(json_encode([&$arr]));

?>
--EXPECT--
*RECURSION*
string(20) "[[{"prop":"value"}]]"
