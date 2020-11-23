--TEST--
Large invalid octal number with underscores
--SKIPIF--
<?php if (!extension_loaded("tokenizer")) print "skip tokenizer extension not enabled"; ?>
--FILE--
<?php

var_dump(token_get_all("<?php 0_10000000000000000000009;"));

?>
--EXPECTF--
array(3) {
  [0]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(6) "<?php "
    [2]=>
    int(1)
  }
  [1]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(25) "0_10000000000000000000009"
    [2]=>
    int(1)
  }
  [2]=>
  string(1) ";"
}
