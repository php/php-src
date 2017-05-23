--TEST--
UUID::NamespaceX500
--CREDITS--
Richard Fussenegger php@fleshgrinder.com
--FILE--
<?php

$uuid = UUID::NamespaceX500();

var_dump(
    $uuid->getVariant() === UUID::VARIANT_RFC4122,
    $uuid->getVersion() === UUID::VERSION_1_TIME_BASED,
    $uuid->toBinary() === "\x6b\xa7\xb8\x14\x9d\xad\x11\xd1\x80\xb4\x00\xc0\x4f\xd4\x30\xc8",
    $uuid->toHex(),
    $uuid->toString()
);

?>
--EXPECT--
bool(true)
bool(true)
bool(true)
string(32) "6ba7b8149dad11d180b400c04fd430c8"
string(36) "6ba7b814-9dad-11d1-80b4-00c04fd430c8"
