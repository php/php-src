--TEST--
SPL: EmptyIterator
--SKIPIF--
<?php if (!extension_loaded("spl")) print "skip"; ?>
--FILE--
<?php

echo "===EmptyIterator===\n";

foreach(new LimitIterator(new EmptyIterator(), 0, 3) as $key => $val)
{
	echo "$key=>$val\n";
}

?>
===DONE===
<?php exit(0);
--EXPECTF--
===EmptyIterator===
===DONE===
