--TEST--
#26802 (Can't call static method using a variable)
--FILE--
<?php
	
class foo
{
	static public function bar() {
		print "baz\n";
	}
}

foo::bar();

$static_method = "foo::bar";

$static_method();

?>
===DONE===
--EXPECT--
baz
===DONE===
