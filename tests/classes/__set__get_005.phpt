--TEST--
ZE2 __set() and __get()
--FILE--
<?php
class Test
{
	protected $x;

	function __get($name) {
		echo __METHOD__ . "\n";
		if (isset($this->x[$name])) {
			return $this->x[$name];
		}
		else
		{
			return NULL;
		}
	}

	function __set($name, $val) {
		echo __METHOD__ . "\n";
		$this->x[$name] = $val;
	}
}

class AutoGen
{
	protected $x;

	function __get($name) {
		echo __METHOD__ . "\n";
		if (!isset($this->x[$name])) {
			$this->x[$name] = new Test();
		}
		return $this->x[$name];
	}

	function __set($name, $val) {
		echo __METHOD__ . "\n";
		$this->x[$name] = $val;
	}
}

$foo = new AutoGen();
$foo->bar->baz = "Check";

var_dump($foo->bar);
var_dump($foo->bar->baz);

?>
===DONE===
--EXPECTF--
AutoGen::__get
Test::__set
AutoGen::__get
object(Test)#%d (1) {
  ["x":protected]=>
  array(1) {
    ["baz"]=>
    string(5) "Check"
  }
}
AutoGen::__get
Test::__get
string(5) "Check"
===DONE===
