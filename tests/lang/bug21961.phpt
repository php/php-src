--TEST--
Bug #21961 (get_parent_class() segfault)
--SKIPIF--
<?php if (version_compare(zend_version(),'2.0.0-dev','<')) die('skip prepared for ZE2'); ?>
--FILE--
<?php

class man
{
	var $name, $bars;
	function man()
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
	var $name;

	function bar($w)
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
		return eval($who.'::getName()');
	}
}

$x = new man;
$x->getdrunk('The old Tavern');
var_dump($x->bars[0]->whosdrunk());
?>
--EXPECT--
string(14) "The old Tavern"
