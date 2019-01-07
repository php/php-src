--TEST--
Test typed properties error condition (type mismatch)
--FILE--
<?php
new class("PHP 7 is better than you, and it knows it ...") {
	public int $int;

	public function __construct(string $string) {
		$this->int = $string;
	}
};
?>
--EXPECTF--
Fatal error: Uncaught TypeError: Typed property class@anonymous::$int must be int, string used in %s:6
Stack trace:
#0 %s(2): class@anonymous->__construct('PHP 7 is better...')
#1 {main}
  thrown in %s on line 6
