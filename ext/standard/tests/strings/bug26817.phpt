--TEST--
Bug #26817 (http_build_query() did not handle private & protected object properties)
--FILE--
<?php
class test { 
	protected $foo;
	private $bar;
	public $test;

	function foo()
	{
		$this->bar = 'meuh';
		$this->foo = 'lala';
		$this->test = 'test';

		var_dump(http_build_query($this));
	}
}

$obj = new test();
$obj->foo();
var_dump(http_build_query($obj));
?>
--EXPECT--
string(27) "foo=lala&bar=meuh&test=test"
string(9) "test=test"
