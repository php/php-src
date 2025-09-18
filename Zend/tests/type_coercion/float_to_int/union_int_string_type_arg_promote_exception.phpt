--TEST--
Promote deprecation warning for int|string type into exception
--FILE--
<?php

function test(int|string $arg) {}

set_error_handler(function($_, $msg) {
    throw new Exception($msg);
});

try {
    test(0.5);
} catch (Exception $e) {
    echo $e->getMessage(), "\n";
}

?>
--EXPECT--
Implicit conversion from float 0.5 to int loses precision
