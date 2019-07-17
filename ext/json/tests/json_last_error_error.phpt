--TEST--
json_last_error() failures
--SKIPIF--
<?php if (!extension_loaded("json")) print "skip"; ?>
--FILE--
<?php

var_dump(json_last_error());

try {
    var_dump(json_last_error(true));
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}

?>
--EXPECT--
int(0)
json_last_error() expects exactly 0 parameters, 1 given
