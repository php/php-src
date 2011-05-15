--TEST--
Testing array dereference on __invoke() result
--FILE--
<?php

error_reporting(E_ALL);

class foo {
	public $x = array();
	public function __construct() {
		$h = array();
		$h[] = new stdclass;
		$this->x = $h;
	}
	public function __invoke() {
		return $this->x;
	}
}


$fo = new foo;
var_dump($fo()[0]);

?>
--EXPECTF--
object(stdClass)#%d (0) {
}
