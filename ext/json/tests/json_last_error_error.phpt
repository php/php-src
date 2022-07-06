--TEST--
json_last_error() failures
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
json_last_error() expects exactly 0 arguments, 1 given
