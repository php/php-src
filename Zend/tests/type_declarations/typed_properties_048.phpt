--TEST--
Nullable typed property
--FILE--
<?php
class Foo {
	public ?int $foo = null;
}
$x = new Foo();
var_dump($x);
try {
	var_dump($x->foo);
} catch (Throwable $e) {
	echo $e->getMessage()."\n";
}
try {
	$x->foo = 5;
} catch (Throwable $e) {
	echo $e->getMessage()."\n";
}
try {
	var_dump($x->foo);
} catch (Throwable $e) {
	echo $e->getMessage()."\n";
}
try {
	$x->foo = null;
} catch (Throwable $e) {
	echo $e->getMessage()."\n";
}
try {
	var_dump($x->foo);
} catch (Throwable $e) {
	echo $e->getMessage()."\n";
}
try {
	unset($x->foo);
} catch (Throwable $e) {
	echo $e->getMessage()."\n";
}
try {
	var_dump($x->foo);
} catch (Throwable $e) {
	echo $e->getMessage()."\n";
}
try {
	$x->foo = "ops";
} catch (Throwable $e) {
	echo $e->getMessage()."\n";
}
?>
--EXPECTF--
object(Foo)#1 (1) {
  ["foo"]=>
  NULL
}
NULL
int(5)
NULL
Typed property Foo::$foo must not be accessed before initialization
Typed property Foo::$foo must be integer or null, string used
