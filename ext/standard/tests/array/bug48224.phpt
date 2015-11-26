--TEST--
Bug #48224 (array_rand no longer shuffles)
--FILE--
<?php
$a = range(0, 100);
$a1 = array_rand($a, count($a));
$a2 = array_rand($a, count($a));
$a3 = array_rand($a, count($a));
$a4 = array_rand($a, count($a));

var_dump($a1 === $a2 && $a1 === $a3 && $a1 === $a4);

?>
--EXPECT--
bool(true)
