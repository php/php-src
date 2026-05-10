--TEST--
Diamond: same args via two paths is accepted
--FILE--
<?php
interface Box<T> {}
interface ABox extends Box<int> {}
interface BBox extends Box<int> {}

class C implements ABox, BBox {}
echo "OK\n";
?>
--EXPECT--
OK
