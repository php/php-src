--TEST--
array_diff() memory leak with custom type checks
--FILE--
<?php

try {
    array_diff([123], 'x');
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}

?>
--EXPECT--
array_diff(): Argument #2 must be of type array, string given
