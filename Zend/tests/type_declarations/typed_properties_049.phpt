--TEST--
Nullable typed property
--FILE--
<?php
class Foo {
	public int $foo = null;
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
Fatal error: Default value for properties with int type can only be int in %styped_properties_049.php on line 3
