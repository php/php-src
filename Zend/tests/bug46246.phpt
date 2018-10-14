--TEST--
Bug #46246 (difference between call_user_func(array($this, $method)) and $this->$method())
--FILE--
<?php
class A
{
	private function Test()
	{
		echo 'Hello from '.get_class($this)."\n";
	}

	public function call($method, $args = array())
	{
		$this->Test();
		$this->$method();
		call_user_func(array($this, $method));
	}
}

class B extends A
{
	protected function Test()
	{
		echo 'Overridden hello from '.get_class($this)."\n";
	}
}

$a = new A;
$b = new B;

$a->call('Test');
$b->call('Test');
?>
--EXPECT--
Hello from A
Hello from A
Hello from A
Hello from B
Hello from B
Hello from B
