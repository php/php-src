--TEST--
token_get_all() and wrong parameters
--EXTENSIONS--
tokenizer
--FILE--
<?php

var_dump(token_get_all(""));
var_dump(token_get_all(0));
var_dump(token_get_all(-1));

echo "Done\n";
?>
--EXPECTF--
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
