--TEST--
Create a class instance from an expression which is a string
--FILE--
<?php

class Foo {
	protected $arg1 = null;
	protected $arg2 = null;

	public function __construct($arg1, $arg2)
	{
		$this->arg1 = $arg1;
		$this->arg2 = $arg2;
	}

	public function getArg1() { return $this->arg1; }
	public function getArg2() { return $this->arg2; }
}

$oo = 'oo';

$foo = new {'f' . $oo}('this is arg1', 'this is arg2');

var_dump(is_object($foo));
echo get_class($foo) . PHP_EOL;
var_dump($foo->getArg1());
var_dump($foo->getArg2());

echo 'Done' . PHP_EOL;
?>
--EXPECTF--	
bool(true)
Foo
string(12) "this is arg1"
string(12) "this is arg2"
Done
