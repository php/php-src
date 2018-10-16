--TEST--
Bug #48215 - parent::method() calls __construct, case sensitive test
--FILE--
<?php
class a {
	public function __CONSTRUCT() { echo __METHOD__ . "\n"; }
	public function a() { echo __METHOD__ . "\n"; }
}
class b extends a {}
class c extends b {
	function __construct() {
		b::b();
	}
}
$c = new c();
?>
===DONE===
--EXPECTF--
Fatal error: Uncaught Error: Call to undefined method b::b() in %s:%d
Stack trace:
#0 %s(%d): c->__construct()
#1 {main}
  thrown in %s on line %d
