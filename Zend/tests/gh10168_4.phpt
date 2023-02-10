--TEST--
GH-10168 (heap-buffer-overflow at zval_undefined_cv): assign typed prop
--FILE--
<?php

class Box {
	public $storage;
}

$box = new Box();

class Test
{
	static ?Test $a = null;

	public function __construct() {
		global $box;

		if (self::$a === null) {
			var_dump($box->storage = $this);
		} else {
			var_dump($box->storage = $this);
		}
	}

	function __destruct() {
		global $box;
		var_dump($box->storage);
		$box->storage = null;
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
object(Test)#3 (0) {
}
NULL
