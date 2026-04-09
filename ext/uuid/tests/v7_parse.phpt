--TEST--
Parse UUID v7 string
--FILE--
<?php

$uuid = Uuid\UuidV7::parse("00000bff-896a-782f-a96a-1b5f4f502630");

var_dump($uuid);

?>
--EXPECTF--
object(Uuid\UuidV7)#%s (%s) {
  ["uuid"]=>
  string(36) "00000bff-896a-782f-a96a-1b5f4f502630"
}
