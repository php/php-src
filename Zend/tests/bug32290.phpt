--TEST--
Bug #32290 (calling call_user_func_array() ends in infinite loop within child class)
--FILE--
<?php

class TestA
{
	public function doSomething($i)
	{
		echo __METHOD__ . "($this)\n";
		return --$i;
	}
}

class TestB extends TestA
{
	public function doSomething($i)
	{
		echo __METHOD__ . "($this)\n";
		$i++;
		if ($i >= 5) return 5;
		return call_user_func_array(array("TestA","doSomething"), array($i));
	}
}

$x = new TestB();
var_dump($x->doSomething(1));

?>
===DONE===
--EXPECTF--
TestB::doSomething(Object id #%d)
TestA::doSomething(Object id #%d)
int(1)
===DONE===
