--TEST--
Recursive bounds: backward reference in a default is allowed
--FILE--
<?php
class Foo {}
class Bar extends Foo {}
class C<T: Foo = Bar, U: Foo = T> {}
echo "ok\n";
?>
--EXPECT--
ok
