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
  string(36) "019b76da-a800-7899-8ef6-b965a5d3744d"
}
