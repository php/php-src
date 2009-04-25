--TEST--
Basic behaviour of call_user_method() test
--CREDITS--
Sebastian Schürmann 
sebs@php.net
Testfest 2009 Munich
--FILE--
<?php
class a {
	static function b() {
		return true;
	}
}
$a = new a();
$res = call_user_method('b', $a);
var_dump($res);
?>
--EXPECTF--
Deprecated: Function call_user_method() is deprecated in %s on line 8
bool(true)
