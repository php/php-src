--TEST--
json_last_error() failures
--FILE--
<?php

var_dump(json_last_error());

try {
    var_dump(json_last_error(true));
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

?>
--EXPECT--
int(0)
ArgumentCountError: json_last_error() expects exactly 0 arguments, 1 given
