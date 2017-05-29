--TEST--
UUID::toHex
--CREDITS--
Richard Fussenegger php@fleshgrinder.com
--FILE--
<?php

use PHP\Std\UUID;

var_dump(UUID::fromBinary("\x00\x01\x02\x03\x04\x05\x06\x07\x08\x09\x0A\x0B\x0C\x0D\x0E\x0F")->toHex());

?>
--EXPECT--
string(32) "000102030405060708090a0b0c0d0e0f"
