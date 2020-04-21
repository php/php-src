--TEST--
Bug #79487 (::getStaticProperties() ignores property modifications)
--FILE--
<?php
class Foo {
    public static $bar = 'orig';
}

Foo::$bar = 'new';
$rc = new ReflectionClass('Foo');
var_dump($rc->getStaticProperties());
?>
--EXPECT--
array(1) {
  ["bar"]=>
  string(3) "new"
}
