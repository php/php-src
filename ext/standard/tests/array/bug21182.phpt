--TEST--
Bug #21182 (range modifies arguments)
--FILE--
<?php
$a = "20"; $b = "30";
echo "a1: $a\n";
$result = range($a, $b);
echo "a2: $a : type : " . gettype($a) . "\n";
?>
--EXPECT--
a1: 20
a2: 20 : type : string
