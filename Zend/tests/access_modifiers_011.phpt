--TEST--
__call() for private/protected methods
--FILE--
<?php

class A {
	private   $var1 = 'var1 value';
	protected $var2 = 'var2 value';

	private function func1()
	{
		return "in func1";
	}
	protected function func2()
	{
		return "in func2";
	}
	public function __get($var)
	{
		return $this->$var;
	}
	public function __call($func, array $args = array())
	{
		return call_user_func_array(array($this, $func), $args);
	}
}

$a = new A();
echo $a->var1,"\n";
echo $a->var2,"\n";
echo $a->func1(),"\n";
echo $a->func2(),"\n";

?>
--EXPECT--
var1 value
var2 value
in func1
in func2
