--TEST--
SPL: DirectoryIterator and clone
--FILE--
<?php
$a = new DirectoryIterator(__DIR__);
$b = clone $a;

$bValue = (string)$b;
$aValue = (string)$a;
if ($aValue != $bValue) {
	echo "aValue and bValue should be the same".PHP_EOL;
	echo $aValue.PHP_EOL;
	echo $bValue.PHP_EOL;
}

var_dump($a->key(), $b->key());
$a->next();
$a->next();
$a->next();
$c = clone $a;

$cValue = (string)$c;
$aValue = (string)$a;
if ($aValue != $cValue) {
	echo "aValue and cValue should be the same".PHP_EOL;
	echo $aValue.PHP_EOL;
	echo $cValue.PHP_EOL;
}

var_dump($a->key(), $c->key());

?>
===DONE===
--EXPECTF--
int(0)
int(0)
int(3)
int(3)
===DONE===
