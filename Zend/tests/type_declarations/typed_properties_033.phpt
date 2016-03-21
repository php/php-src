--TEST--
Test typed properties yield reference guard
--FILE--
<?php
$foo = new class {
	public array $bar = [1,2,3,4,5];

	public function fetch() {
		yield [&$this->bar];
	}
};

foreach ($foo->fetch() as $prop);
--EXPECTF--
Fatal error: Uncaught TypeError: Typed property class@anonymous::$bar must not be referenced in %s:6
Stack trace:
#0 %s(10): class@anonymous->fetch()
#1 {main}
  thrown in %s on line 6
