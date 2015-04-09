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
	function C() {
		b::b();
	}
}
$c = new c();
?>
===DONE===
--EXPECTF--
Fatal error: Call to undefined method b::b() in %s on line %d
