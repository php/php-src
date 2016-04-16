--TEST--
array_column() respects property visibility
--FILE--
<?php

class Test {
    private $prop;
    public function __construct($value) {
        $this->prop = $value;
    }
    public function __isset($name) {
        return true;
    }
    public function __get($name) {
        return "__get($this->prop)";
    }
}

$arr = [new Test("foobar")];
var_dump(array_column($arr, "prop"));

?>
--EXPECT--
array(1) {
  [0]=>
  string(13) "__get(foobar)"
}
