--TEST--
Trying to use lambda as array key
--FILE--
<?php

try {
    var_dump(array(function() { } => 1));
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

?>
--EXPECT--
TypeError: Cannot access offset of type Closure on array
