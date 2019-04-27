--TEST--
Testing invalid method names with __call and __callstatic
--FILE--
<?php

class foo {
	public function __call($a, $b) {
		print "non-static - ok\n";
	}

	public static function __callstatic($a, $b) {
		print "static - ok\n";
	}
}

$a = new foo;
$a->foooo();
$a::foooo();

$b = 'aaaaa1';
$a->$b();
$a::$b();

$b = '  ';
$a->$b();
$a::$b();

$b = str_repeat('a', 10000);
$a->$b();
$a::$b();

$b = NULL;
$a->$b();

?>
--EXPECTF--
non-static - ok
static - ok
non-static - ok
static - ok
non-static - ok
static - ok
non-static - ok
static - ok

Fatal error: Uncaught Error: Method name must be a string in %s:%d
Stack trace:
#0 {main}
  thrown in %s on line %d
