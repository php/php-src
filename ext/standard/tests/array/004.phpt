--TEST--
Test natsort and natcasesort
--POST--
--GET--
--INI--
precision=14
--FILE--
<?php
$data = array(
	'Test1',
	'teST2'=>0,
	5=>'test2',
	'abc'=>'test10',
	'test21'
);

var_dump($data);

natsort($data);
var_dump($data);

natcasesort($data);
var_dump($data);
?>
--EXPECT--
array(5) {
  [0]=>
  string(5) "Test1"
  ["teST2"]=>
  int(0)
  [5]=>
  string(5) "test2"
  ["abc"]=>
  string(6) "test10"
  [6]=>
  string(6) "test21"
}
array(5) {
  ["teST2"]=>
  int(0)
  [0]=>
  string(5) "Test1"
  [5]=>
  string(5) "test2"
  ["abc"]=>
  string(6) "test10"
  [6]=>
  string(6) "test21"
}
array(5) {
  ["teST2"]=>
  int(0)
  [0]=>
  string(5) "Test1"
  [5]=>
  string(5) "test2"
  ["abc"]=>
  string(6) "test10"
  [6]=>
  string(6) "test21"
}