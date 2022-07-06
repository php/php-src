--TEST--
list() can be used to destructure to string offsets, __set and ArrayAccess::offsetSet
--FILE--
<?php

class Obj {
    public $values = [];
    public function __set($name, $value) {
        $this->values[$name] = $value;
    }
}

class Arr implements ArrayAccess {
    public $values = [];
    public function offsetSet($name, $value) {
        $this->values[$name] = $value;
    }
    public function offsetGet($name) {}
    public function offsetExists($name) {}
    public function offsetUnset($name) {}
}

$str = 'ab';
list($str[0], $str[1]) = ['x', 'y'];
var_dump($str);

$obj = new Obj;
list($obj->foo, $obj->bar) = ['foo', 'bar'];
var_dump($obj->values);

$arr = new Arr;
list($arr['foo'], $arr['bar']) = ['foo', 'bar'];
var_dump($arr->values);

?>
--EXPECT--
string(2) "xy"
array(2) {
  ["foo"]=>
  string(3) "foo"
  ["bar"]=>
  string(3) "bar"
}
array(2) {
  ["foo"]=>
  string(3) "foo"
  ["bar"]=>
  string(3) "bar"
}
