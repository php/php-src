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
--EXPECTF--	
array(2) {
  [u"fooStatic"]=>
  unicode(3) "foo"
  [u"foo"]=>
  unicode(3) "foo"
}
Done
