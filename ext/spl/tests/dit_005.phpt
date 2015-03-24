--TEST--
SPL: FilesystemIterator and clone
--FILE--
<?php
$a = new FileSystemIterator(__DIR__);
$b = clone $a;
$bValue = (string)$b;
$aValue = (string)$a;
if ($aValue != $bValue) {
	echo "aValue and bValue should be the same".PHP_EOL;
	echo $aValue.PHP_EOL;
	echo $bValue.PHP_EOL;
}

$aKey = $a->key();
$bKey = $b->key();
if ($aKey != $bKey) {
	echo "aKey and bKey should be the same".PHP_EOL;
	echo $aKey.PHP_EOL;
	echo $bKey.PHP_EOL;
}

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

$aKey = $a->key();
$cKey = $c->key();
if ($aKey != $cKey) {
	echo "aKey and cKey should be the same".PHP_EOL;
	echo $aKey.PHP_EOL;
	echo $cKey.PHP_EOL;
}
?>
===DONE===
--EXPECTF--
===DONE===
