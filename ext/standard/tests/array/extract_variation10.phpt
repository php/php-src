--TEST--
Test extract() function - ensure EXTR_REFS doesn't mess with isRef flag on COW references to array elements.
--FILE--
<?php
$a = array('foo' => 'original.foo');
$nonref = $a['foo'];
$ref = &$a;
extract($a, EXTR_REFS);
$a['foo'] = 'changed.foo';
var_dump($nonref);
?>
--EXPECTF--
string(12) "original.foo"
