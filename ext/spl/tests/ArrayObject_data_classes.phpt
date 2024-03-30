--TEST--
SPL: ArrayObject disallow data classes
--FILE--
<?php

data class DC {}

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
Data object of type DC is not compatible with ArrayObject
Data object of type DC is not compatible with ArrayObject
