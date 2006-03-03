--TEST--
labeled break 11: foreach Iterator
--FILE--
<?php

class MyIterator implements Iterator
{
	protected $ar = array(1,2,3);
	function rewind()
	{
		reset($this->ar);
	}
	function valid()
	{
		return key($this->ar) !== NULL;
	}
	function current()
	{
		return current($this->ar);
	}
	function key()
	{
		return key($this->ar);
	}
	function next()
	{
		next($this->ar);
	}
}

Lend:
foreach(new MyIterator as $k1 => $v1)
{
	echo "$k1=>$v1\n";
	foreach(new MyIterator as $k2 => $v2)
	{
		echo "$k2=>$v2\n";
		if ($v2 == 2)
		{
			if ($v1==2)
			{
				break Lend;
			}
			break;
		}
	}
}

?>
===DONE===
--EXPECTF--
0=>1
0=>1
1=>2
1=>2
0=>1
1=>2
===DONE===
