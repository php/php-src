--TEST--
GH-10168 (heap-buffer-overflow at zval_undefined_cv): assign global variation
--FILE--
<?php

$a = null;

class Test
{
	public function __construct() {
		var_dump($GLOBALS['a'] = $this);
		// Destructor called after comparison, so a will be NULL
		var_dump($GLOBALS['a']);
	}

	function __destruct() {
		unset($GLOBALS['a']);
	}
}
new Test();
new Test();

?>
--EXPECTF--
object(Test)#1 (0) {
}
object(Test)#1 (0) {
}
object(Test)#2 (0) {
}

Warning: Undefined global variable $a in %s on line %d
NULL
