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
Cannot access offset of type Closure on array
