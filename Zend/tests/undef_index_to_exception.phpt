--TEST--
Converting undefined index/offset notice to exception
--FILE--
<?php

set_error_handler(function($_, $msg) {
    throw new Exception($msg);
});

$test = [];
try {
    $test[0] .= "xyz";
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}
var_dump($test);

try {
    $test["key"] .= "xyz";
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}
var_dump($test);

unset($test);
try {
    $GLOBALS["test"] .= "xyz";
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}
try {
    var_dump($test);
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

?>
--EXPECT--
Exception: Undefined array key 0
array(0) {
}
Exception: Undefined array key "key"
array(0) {
}
Exception: Undefined global variable $test
Exception: Undefined variable $test
