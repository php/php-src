--TEST--
json_last_error_msg() failures
--SKIPIF--
<?php if (!extension_loaded("json")) print "skip"; ?>
--FILE--
<?php

var_dump(json_last_error_msg());

try {
    var_dump(json_last_error_msg(true));
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}

?>
--EXPECTF--
string(8) "No error"
json_last_error_msg() expects exactly 0 parameters, 1 given
