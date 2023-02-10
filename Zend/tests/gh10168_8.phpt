--TEST--
GH-10168 (heap-buffer-overflow at zval_undefined_cv): assign typed prop
--FILE--
<?php

class Box {
	public ?Test $storage = null;
}

$box = new Box();

class Test
{
	public function __construct() {
		global $box;
		var_dump($box->storage = $this);
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
