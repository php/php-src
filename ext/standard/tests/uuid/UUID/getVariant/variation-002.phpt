--TEST--
UUID::getVariant ignores Msb2 of octet 8 if Msb0 is 1 and Msb1 is 0
--CREDITS--
Richard Fussenegger php@fleshgrinder.com
--FILE--
<?php

var_dump(
	UUID::fromBinary("\x00\x00\x00\x00\x00\x00\x00\x00\xA0\x00\x00\x00\x00\x00\x00\x00")->getVariant(),
	UUID::fromBinary("\x00\x00\x00\x00\x00\x00\x00\x00\x80\x00\x00\x00\x00\x00\x00\x00")->getVariant()
);

?>
--EXPECT--
int(1)
int(1)
