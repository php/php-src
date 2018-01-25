--TEST--
Bug #68817 (Null pointer deference)
--SKIPIF--
<?php require 'skipif.inc'; ?>
--FILE--
<?php

var_dump(json_decode('[""]'));

?>
===DONE===
--EXPECTF--
array(1) {
  [0]=>
  string(0) ""
}
===DONE===
