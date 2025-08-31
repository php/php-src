--TEST--
ReflectionMethod::__toString() tests (overridden method)
--FILE--
<?php
class Foo {
    function func() {
    }
}
class Bar extends Foo {
    function func() {
    }
}
$m = ReflectionMethod::createFromMethodName("Bar::func");
echo $m;
?>
--EXPECTF--
Method [ <user, overwrites Foo, prototype Foo> public method func ] {
  @@ %s010.php 7 - 8
}
