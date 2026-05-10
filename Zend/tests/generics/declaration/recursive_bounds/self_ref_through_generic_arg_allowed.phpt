--TEST--
Recursive bounds: self-reference inside a generic type argument is allowed (pre-existing behavior)
--FILE--
<?php
class Box<T> {}
class Foo<T: Box<T>> {}
echo "ok\n";
?>
--EXPECT--
ok
