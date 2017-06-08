--TEST--
An anonymous class can implement an interface, extend a class, and use a trait.
--FILE--
<?php

abstract class Foo {
	public function sayFoo() {
		echo "foo\n";
	}
}

interface Bar {
	public function sayBar();
}

trait Baz {
	public function sayBaz() {
		echo "baz\n";
	}
}

$fooBarBaz = new class extends Foo implements Bar {
	use Baz;

	public function sayBar() {
		echo "bar\n";
	}

	public function sayFooBarBaz() {
		$this->sayFoo();
		$this->sayBar();
		$this->sayBaz();
	}
};

echo $fooBarBaz instanceof Foo . "\n";
echo $fooBarBaz instanceof Bar . "\n";
echo (class_uses($fooBarBaz) === ['Baz' => 'Baz']) . "\n";

$fooBarBaz->sayFooBarBaz();
?>
--EXPECTF--
1
1
1
foo
bar
baz