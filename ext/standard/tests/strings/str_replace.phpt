--TEST--
str_replace() tests
--FILE--
<?php

var_dump(str_replace("", "", ""));

var_dump(str_replace("e", "b", "test"));

var_dump(str_replace("", "", "", $count));
var_dump($count);

var_dump(str_replace("q", "q", "q", $count));
var_dump($count);

var_dump(str_replace("long string here", "", "", $count));
var_dump($count);

var_dump(str_replace(chr(0), "a", "", $count));
var_dump($count);

var_dump(str_replace(chr(0), "a", chr(0), $count));
var_dump($count);

var_dump(str_replace("multi", "a", "aaa", $count));
var_dump($count);

var_dump(str_replace("a", "multi", "aaa", $count));
var_dump($count);

var_dump(str_replace(array("a", "a", "b"), "multi", "aaa", $count));
var_dump($count);

var_dump(str_replace(array("a", "a", "b"), array("q", "q", "c"), "aaa", $count));
var_dump($count);

var_dump(str_replace(array("a", "a", "b"), array("q", "q", "c"), array("aaa", "bbb"), $count));
var_dump($count);

var_dump(str_replace("a", array("q", "q", "c"), array("aaa", "bbb"), $count));
var_dump($count);

var_dump(str_replace("a", 1, array("aaa", "bbb"), $count));
var_dump($count);

var_dump(str_replace(1, 3, array("aaa1", "2bbb"), $count));
var_dump($count);

$fp = fopen(__FILE__, "r");
var_dump(str_replace($fp, $fp, $fp, $fp));
var_dump($fp);

echo "Done\n";
?>
--EXPECTF--	
string(0) ""
string(4) "tbst"
string(0) ""
int(0)
string(1) "q"
int(1)
string(0) ""
int(0)
string(0) ""
int(0)
string(1) "a"
int(1)
string(3) "aaa"
int(0)
string(15) "multimultimulti"
int(3)
string(15) "multimultimulti"
int(3)
string(3) "qqq"
int(3)
array(2) {
  [0]=>
  string(3) "qqq"
  [1]=>
  string(3) "ccc"
}
int(6)

Notice: Array to string conversion in %s on line %d
array(2) {
  [0]=>
  string(15) "ArrayArrayArray"
  [1]=>
  string(3) "bbb"
}
int(3)
array(2) {
  [0]=>
  string(3) "111"
  [1]=>
  string(3) "bbb"
}
int(3)
array(2) {
  [0]=>
  string(4) "aaa3"
  [1]=>
  string(4) "2bbb"
}
int(1)
string(%d) "Resource id #%d"
int(1)
Done
