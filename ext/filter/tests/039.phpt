--TEST--
filter_var_array() and different arguments
--SKIPIF--
<?php if (!extension_loaded("filter")) print "skip"; ?>
--FILE--
<?php

var_dump(filter_var_array(NULL));
var_dump(filter_var_array(array()));
var_dump(filter_var_array(array(1,"blah"=>"hoho")));
var_dump(filter_var_array(array(), -1));
var_dump(filter_var_array(array(), 1000000));
var_dump(filter_var_array(array(), ""));

var_dump(filter_var_array(array(""=>""), -1));
var_dump(filter_var_array(array(""=>""), 1000000));
var_dump(filter_var_array(array(""=>""), ""));

var_dump(filter_var_array(array("aaa"=>"bbb"), -1));
var_dump(filter_var_array(array("aaa"=>"bbb"), 1000000));
var_dump(filter_var_array(array("aaa"=>"bbb"), ""));

var_dump(filter_var_array(array(), new stdclass));
var_dump(filter_var_array(array(), array()));
var_dump(filter_var_array(array(), array("var_name"=>1)));
var_dump(filter_var_array(array(), array("var_name"=>-1)));
var_dump(filter_var_array(array("var_name"=>""), array("var_name"=>-1)));
var_dump(filter_var_array(array("var_name"=>""), array("var_name"=>-1, "asdas"=>"asdasd", "qwe"=>"rty", ""=>"")));
var_dump(filter_var_array(array("asdas"=>"text"), array("var_name"=>-1, "asdas"=>"asdasd", "qwe"=>"rty", ""=>"")));


$a = array(""=>""); $b = -1;
var_dump(filter_var_array($a, $b));
var_dump($a, $b);

$a = array(""=>""); $b = 100000;
var_dump(filter_var_array($a, $b));
var_dump($a, $b);

$a = array(""=>""); $b = "";
var_dump(filter_var_array($a, $b));
var_dump($a, $b);

echo "Done\n";
?>
--EXPECTF--	
Warning: filter_var_array() expects parameter 1 to be array, null given in %s on line %d
NULL
array(0) {
}
array(2) {
  [0]=>
  string(1) "1"
  ["blah"]=>
  string(4) "hoho"
}
array(0) {
}
array(0) {
}
bool(false)
array(1) {
  [""]=>
  string(0) ""
}
array(1) {
  [""]=>
  string(0) ""
}
bool(false)
array(1) {
  ["aaa"]=>
  string(3) "bbb"
}
array(1) {
  ["aaa"]=>
  string(3) "bbb"
}
bool(false)
bool(false)
array(0) {
}
array(1) {
  ["var_name"]=>
  NULL
}
array(1) {
  ["var_name"]=>
  NULL
}
array(1) {
  ["var_name"]=>
  string(0) ""
}
array(4) {
  ["var_name"]=>
  string(0) ""
  ["asdas"]=>
  NULL
  ["qwe"]=>
  NULL
  [""]=>
  NULL
}
array(4) {
  ["var_name"]=>
  NULL
  ["asdas"]=>
  string(4) "text"
  ["qwe"]=>
  NULL
  [""]=>
  NULL
}
array(1) {
  [""]=>
  string(0) ""
}
array(1) {
  [""]=>
  string(0) ""
}
int(-1)
array(1) {
  [""]=>
  string(0) ""
}
array(1) {
  [""]=>
  string(0) ""
}
int(100000)
bool(false)
array(1) {
  [""]=>
  string(0) ""
}
string(0) ""
Done
