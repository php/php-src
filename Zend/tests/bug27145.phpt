--TEST--
Bug #27145 (Errors messages print unmangled PPP property names)
--FILE--
<?php
class a {
	protected static $v = "aa";
}
class b extends a {
	protected static $v = "bb";
}
?>
--EXPECTF--
Fatal error: Cannot redeclare property static protected a::$v in class b in %s on line %d