--TEST--
Trying to use lambda in array offset
--FILE--
<?php

try {
    $test[function(){}] = 1;
} catch (Error $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

?>
--EXPECT--
TypeError: Cannot access offset of type Closure on array
