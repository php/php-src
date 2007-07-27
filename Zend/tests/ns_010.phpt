--TEST--
010: Accesing internal namespace class
--FILE--
<?php
namespace X;
import X as Y;
class Foo {
	const C = "const ok\n";
	function __construct() {
		echo "class ok\n";
	}
	static function bar() {
		echo "method ok\n";
	}
}
new Foo();
new X::Foo();
new Y::Foo();
new ::X::Foo();
Foo::bar();
X::Foo::bar();
Y::Foo::bar();
::X::Foo::bar();
echo Foo::C;
echo X::Foo::C;
echo Y::Foo::C;
echo ::X::Foo::C;
--EXPECT--
class ok
class ok
class ok
class ok
method ok
method ok
method ok
method ok
const ok
const ok
const ok
const ok
