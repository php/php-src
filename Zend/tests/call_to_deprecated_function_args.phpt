--TEST--
Check that arguments are freed when calling a deprecated function
--FILE--
<?php

set_error_handler(function($code, $msg) {
    throw new Error($msg);
});

try {
    ezmlm_hash(new stdClass);
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}

$ret = new stdClass;
try {
    $ret = ezmlm_hash(new stdClass);
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}

try {
    $fn = 'ezmlm_hash';
    $fn(new stdClass);
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}

$ret = new stdClass;
try {
    $fn = 'ezmlm_hash';
    $ret = $fn(new stdClass);
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}

?>
--EXPECT--
Function ezmlm_hash() is deprecated
Function ezmlm_hash() is deprecated
Function ezmlm_hash() is deprecated
Function ezmlm_hash() is deprecated
