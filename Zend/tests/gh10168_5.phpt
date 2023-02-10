--TEST--
GH-10168 (heap-buffer-overflow at zval_undefined_cv): assign typed prop
--FILE--
<?php

class Foo {}

class Test
{
	static Test|Foo|null $a = null;

	public function __construct() {
		var_dump(self::$a = $this);
	}

	function __destruct() {
		var_dump(self::$a = new Foo());
	}
}
new Test();
new Test();

?>
--EXPECT--
object(Test)#1 (0) {
}
object(Foo)#3 (0) {
}
object(Test)#2 (0) {
}
object(Foo)#1 (0) {
}
