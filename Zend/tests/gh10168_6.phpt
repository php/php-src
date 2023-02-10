--TEST--
GH-10168 (heap-buffer-overflow at zval_undefined_cv): assign typed prop without cache slot
--XFAIL--
--FILE--
<?php

class Box {
	public $storage;
}

$box = new Box();

class Test
{
	public function __construct() {
		static $i = 0;
		$i++;
		global $box;
		if ($i === 1) {
			var_dump($box->storage = $this);
		} else {
			// Avoid cache slot, triggering write_property
			var_dump($box->storage = $this);
		}
	}

	function __destruct() {
		global $box;
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
