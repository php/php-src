--TEST--
Bug #70478 (**= does no longer work)
--FILE--
<?php

class foo {
	public $a = 3;
	private $b = 4;

	function __construct() {
		$this->a **= $this->b;
	}
}

$a = new foo;
var_dump($a->a);

?>
--EXPECT--
int(81)
