--TEST--
Bug #44653 (Invalid namespace name resolution)
--FILE--
<?php
namespace A;
const XX=1;
function fooBar() { echo __FUNCTION__ . PHP_EOL; }

namespace B;
class A {
	static function fooBar() { echo "bag1\n"; }	
}
class B {
	static function fooBar() { echo "bag2\n"; }
}
function fooBar() { echo __FUNCTION__ . PHP_EOL; }
var_dump(\A\XX);
A::fooBar();
\A\fooBar();
B::fooBar();
fooBar();
\B\fooBar();
?>
--EXPECT--
int(1)
bag1
A\fooBar
bag2
B\fooBar
B\fooBar
