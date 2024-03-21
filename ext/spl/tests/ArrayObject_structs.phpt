--TEST--
SPL: ArrayObject disallow structs
--FILE--
<?php

struct DC {}

$dc = new DC();

try {
    new ArrayObject($dc);
} catch (Exception $e) {
    echo $e->getMessage(), "\n";
}

try {
    $ao = new ArrayObject([]);
    $ao->exchangeArray($dc);
} catch (Exception $e) {
    echo $e->getMessage(), "\n";
}

?>
--EXPECT--
Structs are not compatible with ArrayObject
Structs are not compatible with ArrayObject
