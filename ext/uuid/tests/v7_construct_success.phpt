--TEST--
Instantiate UuidV7 with its constructor
--FILE--
<?php

$uuid = new Uuid\UuidV7("00000bff-896a-782f-a96a-1b5f4f502630");

var_dump($uuid);
?>
--EXPECTF--
object(Uuid\UuidV7)#%d (%d) {
  ["uuid"]=>
  string(36) "00000bff-896a-782f-a96a-1b5f4f502630"
}
