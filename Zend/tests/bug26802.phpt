--TEST--
Bug #26802 (Can't call static method using a variable)
--FILE--
<?php

function func() {
	echo __METHOD__ . "\n";
}

function work() {
	echo __METHOD__ . "\n";
}

$function = 'func';
$function();

class foo
{
	static $method = 'func';

	static public function bar() {
		echo __METHOD__ . "\n";
	}
	
	static public function func() {
		echo __METHOD__ . "\n";
	}
}

foo::bar();

$static_method = "foo::bar";

$static_method();

/* The following is a BC break with PHP4 where it would 
 * call foo::fail. In PHP5 we first evaluate static class 
 * properties and then do the function call.
 */
$method = 'fail';
foo::$method();
?>
===DONE===
--EXPECT--
func
foo::bar
foo::bar
func
===DONE===
