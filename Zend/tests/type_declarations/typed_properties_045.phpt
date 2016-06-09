--TEST--
foreach() leaks on object with typed properties
--FILE--
<?php
class Foo {
	public int $bar;
}

try {
	foreach (new Foo() as &$val) {}
} catch (Throwable $e) {
	echo $e->getMessage() . "\n";
}
--EXPECT--
Typed properties exist in Foo: foreach by reference is disallowed
