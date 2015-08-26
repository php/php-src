--TEST--
bug #70321 (Magic getter breaks reference to array property)
--FILE--
<?php
class foo implements arrayAccess
{
	private $bar;
	public function __construct()
	{
		$this->bar = new bar();
	}
	public function & __get($key)
	{
		$bar = $this->bar;
		return $bar;
	}

	public function & offsetGet($key) {
		$bar = $this->bar;
		return $bar;
	}
	public function offsetSet($key, $val) {
	}
	public function offsetUnset($key) {
	}
	public function offsetExists($key) {
	}
}
class bar { public $onBaz = []; }

$foo = new foo();
$foo->bar->onBaz[] = function() {};
var_dump($foo->bar->onBaz);

$foo = new foo();
$foo["bar"]->onBaz[] = function() {};
var_dump($foo->bar->onBaz);
?>
--EXPECTF--
array(1) {
  [0]=>
  object(Closure)#%d (0) {
  }
}
array(1) {
  [0]=>
  object(Closure)#%d (0) {
  }
}
