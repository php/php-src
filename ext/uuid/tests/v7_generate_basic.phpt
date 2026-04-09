--TEST--
Generate UUIDv7 with seed
--FILE--
<?php

$uuid = Uuid\UuidV7::generate(
    new DateTimeImmutable("2026-01-01 00:00:00"),
    new Random\Engine\PcgOneseq128XslRr64("abcdefghijklmnop")
);

var_dump($uuid);

?>
--EXPECTF--
object(Uuid\UuidV7)#%s (%s) {
  ["uuid"]=>
  string(36) "019b76da-a800-7819-8e76-39652553744d"
}
