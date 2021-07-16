--TEST--
Bug #68896 (Changing ArrayObject value cause Segment Fault)
--FILE--
<?php
class A implements ArrayAccess {
    private $a = [];
    function offsetGet($offset): mixed {
        return $this->a[$offset];
    }
        function offsetSet($offset, $value): void {
                $this->a[$offset] = $value;
        }
        function offsetExists($offset): bool {
                isset($this->a[$offset]);
        }
        function offsetUnset($offset): void {
                unset($this->a[$offset]);
        }
}

$obj = new ArrayObject(["a" => 1]);
$obj["a"] .= "test";
var_dump($obj["a"]);

$obj = new A;
$obj["a"] = 1;
$obj["a"] .= "test";
var_dump($obj["a"]);
?>
--EXPECT--
string(5) "1test"
string(5) "1test"
