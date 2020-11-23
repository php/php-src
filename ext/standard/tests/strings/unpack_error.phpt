--TEST--
Test unpack() function : error conditions
--FILE--
<?php

try {
    var_dump(unpack("B", pack("I", 65534)));
} catch (ValueError $e) {
    echo $e->getMessage(), "\n";
}

?>
--EXPECT--
Invalid format type B
