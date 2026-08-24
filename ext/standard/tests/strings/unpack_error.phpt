--TEST--
Test unpack() function : error conditions
--FILE--
<?php

try {
    var_dump(unpack("B", pack("I", 65534)));
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

?>
--EXPECT--
ValueError: Invalid format type B
