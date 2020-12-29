--TEST--
JsonEncoder::encode() test
--SKIPIF--
<?php if (!extension_loaded("simdjson")) { print "skip"; } ?>
--FILE--
<?php

//var_dump(JsonEncoder::encode("foo"));

?>
--EXPECT--
