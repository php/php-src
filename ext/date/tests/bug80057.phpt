--TEST--
Bug #80057 (DateTimeImmutable::createFromFormat() does not populate time)
--FILE--
<?php
$now = new DateTimeImmutable;
$parsed = DateTimeImmutable::createFromFormat('Y-m-d', '2020-09-04');
$nowStr = $now->format("H:i");
$parsedStr = $parsed->format("H:i");

var_dump($nowStr == $parsedStr);
?>
--EXPECT--
bool(true)
