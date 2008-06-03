--TEST--
Testing __callStatic
--XFAIL--
http://bugs.php.net/bug.php?id=45089
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
ok
ok

Fatal error: Can not call constructor in %s on line %d
