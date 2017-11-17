--TEST--
Bug #21961 (get_parent_class() segfault)
--FILE--
<?php

class man
{
	public $name, $bars;
	function __construct()
	{
		$this->name = 'Mr. X';
		$this->bars = array();
	}

	function getdrunk($where)
	{
		$this->bars[] = new bar($where);
	}

	function getName()
	{
		return $this->name;
	}
}

class bar extends man
{
	public $name;

	function __construct($w)
	{
		$this->name = $w;
	}

	function getName()
	{
		return $this->name;
	}

	function whosdrunk()
	{
		$who = get_parent_class($this);
		if($who == NULL)
		{
			return 'nobody';
		}
		return eval("return ".$who.'::getName();');
	}
}

$x = new man;
$x->getdrunk('The old Tavern');
var_dump($x->bars[0]->whosdrunk());
?>
--EXPECT--
string(14) "The old Tavern"
