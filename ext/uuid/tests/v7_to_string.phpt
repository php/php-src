--TEST--
Convert a UUID v7 to a string
--FILE--
<?php

$uuid = Uuid\UuidV7::parse("00000bff-896a-782f-a96a-1b5f4f502630");

var_dump($uuid->toString());

?>
--EXPECT--
string(36) "00000bff-896a-782f-a96a-1b5f4f502630"
