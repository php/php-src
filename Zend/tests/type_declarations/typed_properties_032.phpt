--TEST--
Test typed properties return by ref is disallowed
--FILE--
<?php
$foo = new class {

	public int $bar = 15;

	public function & method() {
		return $this->bar;
	}
};

var_dump($foo->method());
--EXPECTF--
Fatal error: Uncaught TypeError: Typed property class@anonymous::$bar must not be referenced in %s:7
Stack trace:
#0 %s(11): class@anonymous->method()
#1 {main}
  thrown in %s on line 7

