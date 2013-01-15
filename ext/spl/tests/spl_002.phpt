--TEST--
SPL: Countable
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
