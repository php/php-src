--TEST--
json_last_error_msg() failures
--FILE--
<?php

var_dump(json_last_error_msg());

try {
    var_dump(json_last_error_msg(true));
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}

?>
--EXPECT--
string(8) "No error"
json_last_error_msg() expects exactly 0 arguments, 1 given
