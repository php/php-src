--TEST--
Testing method_exists()
--FILE--
<?php

class bar {
	static public function stat_a2() {
	}	
	static private function stat_b2() {
	}
	static protected function stat_c2() {
	}
	
	private function method_a() {
	}
	protected function method_b() {
	}
	public function method_c() {
	}
}



class baz extends bar {
	static public function stat_a() {
	}	
	static private function stat_b() {
	}
	static protected function stat_c() {
	}
	
	private function method_a() {
	}
	protected function method_b() {
	}
	public function method_c() {
	}
}

var_dump(method_exists('baz', 'stat_a'));
var_dump(method_exists('baz', 'stat_b'));
var_dump(method_exists('baz', 'stat_c'));
print "----\n";
var_dump(method_exists('baz', 'stat_a2'));
var_dump(method_exists('baz', 'stat_b2'));
var_dump(method_exists('baz', 'stat_c2'));
print "----\n";

$baz = new baz;
var_dump(method_exists($baz, 'method_a'));
var_dump(method_exists($baz, 'method_b'));
var_dump(method_exists($baz, 'method_c'));
print "----\n";
var_dump(method_exists($baz, 'stat_a'));
var_dump(method_exists($baz, 'stat_b'));
var_dump(method_exists($baz, 'stat_c'));

?>
--EXPECT--
bool(true)
bool(true)
bool(true)
----
bool(true)
bool(true)
bool(true)
----
bool(true)
bool(true)
bool(true)
----
bool(true)
bool(true)
bool(true)
