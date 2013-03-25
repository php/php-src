--TEST--
Bug #41633.3 (Crash instantiating classes with self-referencing constants)
--FILE--
<?php
class Foo {
	const A = Foo::B;
	const B = Foo::A;
}
echo Foo::A;
?>
--EXPECTF--
Fatal error: Cannot declare self-referencing constant 'Foo::B' in %sbug41633_3.php on line %d
