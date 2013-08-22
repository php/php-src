--TEST--
Bug #65499 (json_decode reports invalid literal as valid JSON)
--SKIPIF--
<?php if (!extension_loaded("json")) print "skip"; ?>
--FILE--
<?php

var_dump(
  json_decode("9af5"),
  json_last_error(),
  json_decode("129hsk"),
  json_last_error()
);

?>
--EXPECT--
NULL
int(4)
NULL
int(4)
