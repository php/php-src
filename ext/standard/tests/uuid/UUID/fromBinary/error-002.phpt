--TEST--
UUID::fromBinary boundaries
--CREDITS--
Richard Fussenegger php@fleshgrinder.com
--FILE--
<?php

foreach ([0, 1, 15, 17, random_int(18, 256)] as $i) {
    try {
        UUID::fromBinary($i === 0 ? '' : random_bytes($i));
    }
    catch (InvalidArgumentException $e) {
        echo $e->getMessage() , "\n";
    }
}

?>
--EXPECTF--
Expected exactly 16 bytes, but got 0
Expected exactly 16 bytes, but got 1
Expected exactly 16 bytes, but got 15
Expected exactly 16 bytes, but got 17
Expected exactly 16 bytes, but got %d
