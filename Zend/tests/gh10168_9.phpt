--TEST--
GH-10168 (heap-buffer-overflow at zval_undefined_cv): assign typed prop
--FILE--
<?php

class Box {
	public $value;
}

$box = new Box();

class Test
{
	static $a = null;

	public function __construct() {
		global $box;
		var_dump($box->value = $this);
	}

	function __destruct() {
		global $box;
		unset($box->value);
	}
}
new Test();
new Test();

?>
--EXPECT--
object(Test)#2 (0) {
}
object(Test)#3 (0) {
}
