--TEST--
Bug #41633.2 (Undefined class constants must not be substituted by strings)
--FILE--
<?php
class Foo {
	const A = self::B;
}
echo Foo::A."\n";
?>
--EXPECTF--
Fatal error: Uncaught Error: Undefined class constant 'self::B' in %sbug41633_2.php:5
Stack trace:
#0 {main}
  thrown in %sbug41633_2.php on line 5
