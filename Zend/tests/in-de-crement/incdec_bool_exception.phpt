--TEST--
Inc/dec on bool: warning converted to exception
--FILE--
<?php

set_error_handler(function($severity, $m) {
    throw new Exception($m, $severity);
});

function inc(&$v) {
    $v++;
}
function dec(&$v) {
    $v--;
}

$values = [false, true];
foreach ($values as $value) {
    try {
        inc($value);
    } catch (\Exception $e) {
        echo $e->getMessage(), PHP_EOL;
    }
    try {
        dec($value);
    } catch (\Exception $e) {
        echo $e->getMessage(), PHP_EOL;
    }
}
?>
--EXPECT--
Increment on type bool has no effect, this will change in the next major version of PHP
Decrement on type bool has no effect, this will change in the next major version of PHP
Increment on type bool has no effect, this will change in the next major version of PHP
Decrement on type bool has no effect, this will change in the next major version of PHP
