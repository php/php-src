--TEST--
Bug GH-9699 (DateTimeImmutable::diff differences in 8.1.10 onwards - timezone related)
--FILE--
<?php

$date = new DateTimeImmutable('2022-10-09 02:41:54.515330', new DateTimeZone('America/Los_Angeles'));
$now = new DateTimeImmutable('2022-10-10 08:41:54.534620', new DateTimeZone('UTC'));

echo $date->diff($now)->format("%R %Y %M %D %H %I %S %F"), "\n";
?>
--EXPECT--
+ 00 00 00 23 00 00 019290
