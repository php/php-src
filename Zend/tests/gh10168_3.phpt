--TEST--
GH-10168 (heap-buffer-overflow at zval_undefined_cv): assign typed static prop
--FILE--
<?php
class Test
{
	static ?Test $a = null;

	public function __construct() {
		static $i = 0;
		$i++;
		if ($i === 1) {
			var_dump(self::$a = $this);
		} else {
			// Avoid cache slot, triggering write_property
			var_dump(self::$a = $this);
		}
	}

	function __destruct() {
		var_dump(self::$a);
		self::$a = null;
	}
}
new Test();
new Test();

?>
--EXPECT--
object(Test)#1 (0) {
}
object(Test)#2 (0) {
}
object(Test)#2 (0) {
}
NULL
