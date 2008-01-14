--TEST--
Bug #42850 array_walk_recursive() leaves references, #34982 array_walk_recursive() modifies elements outside function scope 
--FILE--
<?php

// Bug #42850
$data = array ('key1' => 'val1', array('key2' => 'val2'));
function apply_dumb($item, $key) {}; 
var_dump($data);
array_walk_recursive($data, 'apply_dumb');
$data2 = $data;
$data2[0] = 'altered';
var_dump($data);
var_dump($data2);

// Bug #34982
function myfunc($data) {
    array_walk_recursive($data, 'apply_changed');
}
function apply_changed(&$input, $key) {
    $input = 'changed';
}
myfunc($data);
var_dump($data);

--EXPECT--
array(2) {
  ["key1"]=>
  string(4) "val1"
  [0]=>
  array(1) {
    ["key2"]=>
    string(4) "val2"
  }
}
array(2) {
  ["key1"]=>
  string(4) "val1"
  [0]=>
  array(1) {
    ["key2"]=>
    string(4) "val2"
  }
}
array(2) {
  ["key1"]=>
  string(4) "val1"
  [0]=>
  string(7) "altered"
}
array(2) {
  ["key1"]=>
  string(4) "val1"
  [0]=>
  array(1) {
    ["key2"]=>
    string(4) "val2"
  }
}
