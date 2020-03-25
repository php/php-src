--TEST--
Bug #68817 (Null pointer deference)
--SKIPIF--
<?php if (!extension_loaded("json")) print "skip"; ?>
--FILE--
<?php

var_dump(json_decode('[""]'));

?>
--EXPECT--
array(1) {
  [0]=>
  string(0) ""
}
