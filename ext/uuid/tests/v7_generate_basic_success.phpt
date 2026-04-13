--TEST--
Generate UUIDv7
--FILE--
<?php

$uuid = Uuid\UuidV7::generate();

var_dump($uuid);

?>
--EXPECTF--
object(Uuid\UuidV7)#%s (%s) {
  ["uuid"]=>
  string(36) "%s-%s-%s-%s-%s"
}
