--TEST--
Testing __callStatic
--FILE--
<?php

class foo {
	public function aa() {
		print "ok\n";
	}
	static function __callstatic($a, $b) {
		var_dump($a);
	}
}

foo::aa();

$b = 'AA';
foo::$b();

foo::__construct();

?>
--EXPECTF--
Deprecated: Non-static method foo::aa() should not be called statically in %s on line %d
ok

Deprecated: Non-static method foo::aa() should not be called statically in %s on line %d
ok

Fatal error: Uncaught Error: Cannot call constructor in %s:%d
Stack trace:
#0 {main}
  thrown in %s on line %d
