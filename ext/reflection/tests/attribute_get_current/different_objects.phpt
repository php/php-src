--TEST--
ReflectionAttribute::getCurrent() does not return the same exact ReflectionAttribute instance
--FILE--
<?php

#[Attribute]
class Demo {
	public function __construct() {
		global $originalInstance;
		$currentInstance = ReflectionAttribute::getCurrent();
		assert($originalInstance !== $currentInstance);
		echo "Name:\n";
		echo $originalInstance->name . ' -> ' . $currentInstance->name . "\n";
		echo "Args:\n";
		var_dump($originalInstance->getArguments());
		var_dump($currentInstance->getArguments());
		assert($originalInstance->getArguments() === $currentInstance->getArguments());
	}
}

#[Demo("foo", true, 123)]
class WithDemo {}

$case = new ReflectionClass(WithDemo::class);
$originalInstance = $case->getAttributes()[0];
$originalInstance->newInstance();

?>
--EXPECT--
Name:
Demo -> Demo
Args:
array(3) {
  [0]=>
  string(3) "foo"
  [1]=>
  bool(true)
  [2]=>
  int(123)
}
array(3) {
  [0]=>
  string(3) "foo"
  [1]=>
  bool(true)
  [2]=>
  int(123)
}
