--TEST--
GH-10168 (heap-buffer-overflow at zval_undefined_cv): assign global variation
--FILE--
<?php

$a = null;

class Test
{
	public function __construct() {
		($GLOBALS['a'] = $this) > 0;
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
Notice: Object of class Test could not be converted to int in %s on line %d
object(Test)#1 (0) {
}

Notice: Object of class Test could not be converted to int in %s on line %d

Warning: Undefined global variable $a in %s on line %d
NULL
