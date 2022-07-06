--TEST--
dividing doubles
--INI--
precision=14
--FILE--
<?php

$d1 = 1.1;
$d2 = 434234.234;

$c = $d2 / $d1;
var_dump($c);

$d1 = 1.1;
$d2 = "434234.234";

$c = $d2 / $d1;
var_dump($c);

$d1 = "1.1";
$d2 = "434234.234";

$c = $d2 / $d1;
var_dump($c);

echo "Done\n";
?>
--EXPECT--
float(394758.39454545453)
float(394758.39454545453)
float(394758.39454545453)
Done
