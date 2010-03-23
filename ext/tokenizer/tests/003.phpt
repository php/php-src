--TEST--
token_get_all() and wrong parameters
--SKIPIF--
<?php if (!extension_loaded("tokenizer")) print "skip"; ?>
--FILE--
<?php

var_dump(token_get_all(array()));
var_dump(token_get_all(new stdClass));
var_dump(token_get_all(""));
var_dump(token_get_all(0));
var_dump(token_get_all(-1));

echo "Done\n";
?>
--EXPECTF--	
Warning: token_get_all() expects parameter 1 to be string, array given in %s on line %d
NULL

Warning: token_get_all() expects parameter 1 to be string, object given in %s on line %d
NULL
array(0) {
}
array(1) {
  [0]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(1) "0"
    [2]=>
    int(1)
  }
}
array(1) {
  [0]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(2) "-1"
    [2]=>
    int(1)
  }
}
Done
