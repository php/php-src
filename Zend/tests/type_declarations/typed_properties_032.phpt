--TEST--
Test typed properties return by ref is allowed
--FILE--
<?php
$foo = new class {
	public int $bar = 15;

	public function &method() {
		return $this->bar;
	}
};

var_dump($foo->method());
--EXPECT--
int(15)
