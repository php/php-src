--TEST--
Bug #41884 (ReflectionClass::getDefaultProperties() does not handle static attributes)
--FILE--
<?php

class Foo
{
	protected static $fooStatic = 'foo';
	protected $foo = 'foo';
}

$class = new ReflectionClass('Foo');

var_dump($class->getDefaultProperties());

echo "Done\n";
?>
--EXPECT--
array(2) {
  ["fooStatic"]=>
  string(3) "foo"
  ["foo"]=>
  string(3) "foo"
}
Done
