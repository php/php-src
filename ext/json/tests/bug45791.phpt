--TEST--
Bug #45791 (json_decode() does not handle number 0e0)
--SKIPIF--
<?php if(!extension_loaded("json")) die("skip json extension not loaded"); ?>
--FILE--
<?php

var_dump(json_decode('{"zero": 0e0}'));

?>
--EXPECT--
object(stdClass)#1 (1) {
  ["zero"]=>
  float(0)
}
