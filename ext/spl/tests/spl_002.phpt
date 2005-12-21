--TEST--
SPL: Countable
--SKIPIF--
<?php if (!extension_loaded("spl")) print "skip"; ?>
--FILE--
<?php

class Test implements Countable
{
	function count()
	{
		return 4;
	}
};

$a = new Test;

var_dump(count($a));

?>
===DONE===
--EXPECT--
int(4)
===DONE===
