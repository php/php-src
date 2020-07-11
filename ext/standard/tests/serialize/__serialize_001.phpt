--TEST--
__serialize() mechanism (001): Basics
--FILE--
<?php

class Test {
    public $prop;
    public $prop2;
    public function __serialize() {
        return ["value" => $this->prop, 42 => $this->prop2];
    }
    public function __unserialize(array $data) {
        $this->prop = $data["value"];
        $this->prop2 = $data[42];
    }
}

$test = new Test;
$test->prop = "foobar";
$test->prop2 = "barfoo";
var_dump($s = serialize($test));
var_dump(unserialize($s));

?>
--EXPECT--
string(58) "O:4:"Test":2:{s:5:"value";s:6:"foobar";i:42;s:6:"barfoo";}"
object(Test)#2 (2) {
  ["prop"]=>
  string(6) "foobar"
  ["prop2"]=>
  string(6) "barfoo"
}
