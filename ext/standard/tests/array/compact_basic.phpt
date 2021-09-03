--TEST--
Test compact() function : basic functionality
--FILE--
<?php
/*
 * Test basic functionality
 */

echo "*** Testing compact() : basic functionality ***\n";

$a=1;
$b=0.2;
$c=true;
$d=array("key"=>"val");
$e=NULL;
$f="string";

// simple array test
var_dump (compact(array("a", "b", "c", "d", "e", "f")));
// simple parameter test
var_dump (compact("a", "b", "c", "d", "e", "f"));
var_dump (compact(array("keyval"=>"a", "b"=>"b")));
var_dump(compact(array("g")));

echo "Done";
?>
--EXPECTF--
*** Testing compact() : basic functionality ***
array(6) {
  ["a"]=>
  int(1)
  ["b"]=>
  float(0.2)
  ["c"]=>
  bool(true)
  ["d"]=>
  array(1) {
    ["key"]=>
    string(3) "val"
  }
  ["e"]=>
  NULL
  ["f"]=>
  string(6) "string"
}
array(6) {
  ["a"]=>
  int(1)
  ["b"]=>
  float(0.2)
  ["c"]=>
  bool(true)
  ["d"]=>
  array(1) {
    ["key"]=>
    string(3) "val"
  }
  ["e"]=>
  NULL
  ["f"]=>
  string(6) "string"
}
array(2) {
  ["a"]=>
  int(1)
  ["b"]=>
  float(0.2)
}

Warning: compact(): Undefined variable $g in %s on line %d
array(0) {
}
Done
