--TEST--
010: Accesing internal namespace class
--FILE--
<?php
namespace X;
use X as Y;
class Foo {	
	const C = "const ok\n";
	static $var = "var ok\n";
	function __construct() {
		echo "class ok\n";
	}
	static function bar() {
		echo "method ok\n";
	}
}
new Foo();
new Y\Foo();
new \X\Foo();
Foo::bar();
Y\Foo::bar();
\X\Foo::bar();
echo Foo::C;
echo Y\Foo::C;
echo \X\Foo::C;
echo Foo::$var;
echo Y\Foo::$var;
echo \X\Foo::$var;
--EXPECT--
class ok
class ok
class ok
method ok
method ok
method ok
const ok
const ok
const ok
var ok
var ok
var ok
