--TEST--
Testing method name case
--XFAIL--
http://bugs.php.net/bug.php?id=45089
--FILE--
<?php

class Foo {
	public function __call($a, $b) {
		print "nonstatic\n";
		var_dump($a);
	}
	static public function __callStatic($a, $b) {
		print "static\n";
		var_dump($a);
	}
	public function test() {
		$this->fOoBaR();
		self::foOBAr();
		$this::fOOBAr();
	}
}

$a = new Foo;
$a->test();
$a::bAr();
foo::BAZ();

?>
--EXPECT--
nonstatic
unicode(6) "fOoBaR"
nonstatic
unicode(6) "foOBAr"
nonstatic
unicode(6) "fOOBAr"
static
unicode(3) "bAr"
static
unicode(3) "BAZ"
