--TEST--
UUID::toString
--CREDITS--
Richard Fussenegger php@fleshgrinder.com
--FILE--
<?php

use PHP\Std\UUID;

var_dump(UUID::fromBinary("\x00\x01\x02\x03\x04\x05\x06\x07\x08\x09\x0A\x0B\x0C\x0D\x0E\x0F")->toString());

?>
--EXPECT--
string(36) "00010203-0405-0607-0809-0a0b0c0d0e0f"
