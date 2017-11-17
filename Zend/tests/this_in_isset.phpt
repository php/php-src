--TEST--
$this in isset
--FILE--
<?php
var_dump(isset($this));
try {
	var_dump(isset($this->foo));
} catch (Throwable $e) {
	echo "exception\n";
}
try {
	var_dump(isset($this->foo->bar));
} catch (Throwable $e) {
	echo "exception\n";
}
try {
	var_dump(isset($this[0]));
} catch (Throwable $e) {
	echo "exception\n";
}

class A extends ArrayObject {
	public $foo = 5;
	function foo() {
		$this[0] = 5;
		var_dump(isset($this));
		var_dump(isset($this->foo));
		var_dump(isset($this[0]));
	}
}
$a = new A();
$a->foo();
?>
--EXPECT--
bool(false)
exception
exception
exception
bool(true)
bool(true)
bool(true)
