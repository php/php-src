--TEST--
ZE2 __set() and __get()
--SKIPIF--
<?php if (version_compare(zend_version(), '2.0.0-dev', '<')) die('skip ZendEngine 2 needed'); ?>
--FILE--
<?php
class Test {
	protected $x;

	function __get($name) {
		if (isset($this->x[$name])) {
			return $this->x[$name];
		} 
		else
		{
			return NULL;
		}
	}

	function __set($name, $val) {
		$this->x[$name] = $val;
	}
}

$foo = new Test();
$bar = new Test();
$bar->baz = "Check";

$foo->bar = $bar;

var_dump($bar->baz);
var_dump($foo->bar->baz);

?>
===DONE===
--EXPECTF--
string(5) "Check"
string(5) "Check"
===DONE===
