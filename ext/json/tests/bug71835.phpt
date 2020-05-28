--TEST--
Bug #71835 (json_encode sometimes incorrectly detects recursion with JsonSerializable)
--FILE--
<?php
class SomeClass implements JsonSerializable {
    public function jsonSerialize() {
        return [get_object_vars($this)];
    }
}
$class = new SomeClass;
$arr = [$class];
var_dump(json_encode($arr));

class SomeClass2 implements JsonSerializable {
    public function jsonSerialize() {
        return [(array)$this];
    }
}
$class = new SomeClass2;
$arr = [$class];
var_dump(json_encode($arr));
?>
--EXPECT--
string(6) "[[[]]]"
string(6) "[[[]]]"
