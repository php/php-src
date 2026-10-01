--TEST--
GH-18422 (int overflow in Date extension)
--FILE--
<?php
date_default_timezone_set('UTC');

$dto = date_create("2006-12-12");
date_isodate_set($dto, PHP_INT_MIN, 1, 1);
echo $dto->format("Y"), "\n";
echo $dto->format("x"), "\n";
echo $dto->format("X"), "\n";

echo date_create("2024-06-15")->format("Y"), "\n";
echo date_create("-0042-01-01")->format("Y"), "\n";
?>
--EXPECTF--
-%d
-%d
-%d
2024
-0042
