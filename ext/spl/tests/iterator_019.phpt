--TEST--
SPL: NoRewindIterator
--SKIPIF--
<?php if (!extension_loaded("spl")) print "skip"; ?>
--FILE--
<?php

echo "===Current===\n";

$it = new NoRewindIterator(new ArrayIterator(array(0 => 'A', 1 => 'B', 2 => 'C')));

echo $it->key() . '=>' . $it->current() . "\n";

echo "===Next===\n";

$it->next();

echo "===Foreach===\n";

foreach($it as $key=>$val)
{
	echo "$key=>$val\n";
}

?>
===DONE===
<?php exit(0); ?>
--EXPECTF--
===Current===
0=>A
===Next===
===Foreach===
1=>B
2=>C
===DONE===
