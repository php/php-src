--TEST--
Bug #68896 (Changing ArrayObject value cause Segment Fault)
--FILE--
<?php
class A implements ArrayAccess {
	private $a = [];
	function offsetGet($offset) {
		return $this->a[$offset];
	}
        function offsetSet($offset, $value) {
                $this->a[$offset] = $value;
        }
        function offsetExists($offset) {
                isset($this->a[$offset]);
        }
        function offsetUnset($offset) {
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
--EXPECT--
string(5) "1test"
string(5) "1test"
