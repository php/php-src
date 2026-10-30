--TEST--
Properties serialization for SplFixedArray should have updated properties
--FILE--
<?php
#[AllowDynamicProperties]
class MySplFixedArray extends SplFixedArray {
    public $x;
    public int $y = 3;
}

$x = new MySplFixedArray(2);
var_dump($x->y);
$x->y = 2;
var_dump($x->y);
$serialized = serialize($x);
var_dump($serialized);
var_dump(unserialize($serialized));

$x->dynamic_property = "dynamic_property_value";
$serialized = serialize($x);
var_dump($serialized);
var_dump(unserialize($serialized));

$x->dynamic_property = "dynamic_property_value2";
$x->y = 4;
$serialized = serialize($x);
var_dump($serialized);
var_dump(unserialize($serialized));
?>
--EXPECT--
int(3)
int(2)
string(61) "O:15:"MySplFixedArray":4:{i:0;N;i:1;N;s:1:"x";N;s:1:"y";i:2;}"
object(MySplFixedArray)#2 (4) {
  [0]=>
  NULL
  [1]=>
  NULL
  ["x"]=>
  NULL
  ["y"]=>
  int(2)
}
string(115) "O:15:"MySplFixedArray":5:{i:0;N;i:1;N;s:1:"x";N;s:1:"y";i:2;s:16:"dynamic_property";s:22:"dynamic_property_value";}"
object(MySplFixedArray)#2 (5) {
  [0]=>
  NULL
  [1]=>
  NULL
  ["x"]=>
  NULL
  ["y"]=>
  int(2)
  ["dynamic_property"]=>
  string(22) "dynamic_property_value"
}
string(116) "O:15:"MySplFixedArray":5:{i:0;N;i:1;N;s:1:"x";N;s:1:"y";i:4;s:16:"dynamic_property";s:23:"dynamic_property_value2";}"
object(MySplFixedArray)#2 (5) {
  [0]=>
  NULL
  [1]=>
  NULL
  ["x"]=>
  NULL
  ["y"]=>
  int(4)
  ["dynamic_property"]=>
  string(23) "dynamic_property_value2"
}
