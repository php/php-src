--TEST--
Trying to use lambda in array offset
--FILE--
<?php

try {
    $test[function(){}] = 1;
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}
try {
    $a{function() { }} = 1;
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}

?>
--EXPECTF--
Deprecated: Array and string offset access syntax with curly braces is deprecated in %s on line %d
Illegal offset type
Illegal offset type
