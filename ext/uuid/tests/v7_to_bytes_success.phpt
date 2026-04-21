--TEST--
Convert a UUID v7 to bytes
--FILE--
<?php

$uuid = new Uuid\UuidV7("00000bff-896a-782f-a96a-1b5f4f502630");

var_dump($uuid->toBytes());

?>
--EXPECTF--
string(16) "%s"
