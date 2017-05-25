--TEST--
UUID::Nil
--CREDITS--
Richard Fussenegger php@fleshgrinder.com
--FILE--
<?php

$uuid = UUID::Nil();

var_dump(
	$uuid->getVariant(),
	$uuid->getVersion(),
	$uuid->toBinary() === "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0",
	$uuid->toHex(),
	$uuid->toString()
);

?>
--EXPECT--
int(0)
int(0)
bool(true)
string(32) "00000000000000000000000000000000"
string(36) "00000000-0000-0000-0000-000000000000"
