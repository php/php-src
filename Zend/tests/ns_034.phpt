--TEST--
034: Support for namespaces in compile-time constant reference
--FILE--
<?php
namespace A;
use A as B;
class Foo {
	const C = "ok\n";
}
function f1($x=Foo::C) {
	echo $x;
}
function f2($x=A::Foo::C) {
	echo $x;
}
function f3($x=B::Foo::C) {
	echo $x;
}
function f4($x=::A::Foo::C) {
	echo $x;
}
echo Foo::C;
echo A::Foo::C;
echo B::Foo::C;
echo ::A::Foo::C;
f1();
f2();
f3();
f4();
--EXPECT--
ok
ok
ok
ok
ok
ok
ok
ok
