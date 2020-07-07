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
} catch (Exception $e) {
    echo $e->getMessage(), "\n";
}
var_dump($test);

try {
    $test["key"] .= "xyz";
} catch (Exception $e) {
    echo $e->getMessage(), "\n";
}
var_dump($test);

unset($test);
try {
    $GLOBALS["test"] .= "xyz";
} catch (Exception $e) {
    echo $e->getMessage(), "\n";
}
try {
    var_dump($test);
} catch (Exception $e) {
    echo $e->getMessage(), "\n";
}

?>
--EXPECT--
Undefined array offset 0
array(0) {
}
Undefined array index "key"
array(0) {
}
Undefined array index "test"
Undefined variable $test
