--TEST--
Diamond: class implementing two interfaces that bind the same invariant ancestor (accepted, LSP per path validates any actual methods)
--FILE--
<?php
interface Box<T> {}
interface ABox extends Box<int> {}
interface BBox extends Box<string> {}

class C implements ABox, BBox {}
$c = new C();
var_dump($c instanceof Box, $c instanceof ABox, $c instanceof BBox);
?>
--EXPECT--
bool(true)
bool(true)
bool(true)
