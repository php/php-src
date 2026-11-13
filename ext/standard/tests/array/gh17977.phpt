--TEST--
array_any() / array_all() leak
--DESCRIPTION--
Found in GH-16831#issuecomment-2700410631
--EXTENSIONS--
zend_test
--FILE--
<?php

$key = zend_return_non_interned_string('abc');

var_dump(array_any([$key => 1], static fn () => true));
var_dump(array_all([$key => 1], static fn () => false));

?>
--EXPECT--
bool(true)
bool(false)
