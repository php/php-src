--TEST--
Using trait that implements an interface
--FILE--
<?php

interface baz {
	public function abc();
}

trait foo implements baz {
	public function abc() {
	}
}

class bar implements baz {
	use foo;
}

class quux {
	use foo;
}

new bar;
print_r(class_implements(bar::class));
new quux;
print_r(class_implements(quux::class));

print_r(class_implements(foo::class));
print_r((new ReflectionClass(foo::class))->getInterfaceNames());
var_dump((new ReflectionClass(foo::class))->implementsInterface(baz::class));

print "OK\n";

?>
--EXPECT--
Array
(
    [baz] => baz
)
Array
(
)
Array
(
    [baz] => baz
)
Array
(
    [0] => baz
)
bool(true)
OK
