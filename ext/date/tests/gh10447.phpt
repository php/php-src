--TEST--
Bug GH-10447 ('p' format specifier does not yield 'Z' for 00:00)
--FILE--
<?php
$date = new \DateTimeImmutable('2023-01-25T00:00:00+00:00');
echo $date->format('Y-m-d\TH:i:sp'), "\n";

$date = new \DateTimeImmutable('2023-01-25T00:00:00-00:00');
echo $date->format('Y-m-d\TH:i:sp'), "\n";
?>
--EXPECT--
2023-01-25T00:00:00Z
2023-01-25T00:00:00Z
