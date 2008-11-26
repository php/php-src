--TEST--
Test extract() function - ensure EXTR_REFS works when array is referenced and keys clash with variables in current scope. 
--FILE--
<?php
$a = array('foo' => 'original.foo');
$ref = &$a;
$foo = 'test';
extract($a, EXTR_OVERWRITE|EXTR_REFS);
$foo = 'changed.foo';
var_dump($a['foo']);
?>
--EXPECTF--
%unicode|string%(11) "changed.foo"
