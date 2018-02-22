--TEST--
SPL: SplObjectStorage comapred with ==
--FILE--
<?php
$a = new SplObjectStorage;
$b = new SplObjectStorage;
var_dump($a == $b);
$b[$b] = 2;
var_dump($a == $b);
$a[$b] = 2;
var_dump($a == $b);
$a[$b] = 3;
var_dump($a == $b);
?>
===DONE===
<?php exit(0); ?>
--EXPECT--
bool(true)
bool(false)
bool(true)
bool(false)
===DONE===
