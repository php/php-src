--TEST--
Trying to use lambda in array offset
--FILE--
<?php

try {
    $test[function(){}] = 1;
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}

?>
--EXPECT--
Illegal offset type
