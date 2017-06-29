--TEST--
UUID::getVersion
--CREDITS--
Richard Fussenegger php@fleshgrinder.com
--FILE--
<?php

use PHP\Std\UUID;

for ($i = 0; $i < 16; ++$i) {
	var_dump(UUID::fromBinary(sprintf("\x00\x00\x00\x00\x00\x00%s\x00\x00\x00\x00\x00\x00\x00\x00\x00", chr($i << 4)))->getVersion());
}

?>
--EXPECT--
int(0)
int(1)
int(2)
int(3)
int(4)
int(5)
int(6)
int(7)
int(8)
int(9)
int(10)
int(11)
int(12)
int(13)
int(14)
int(15)
