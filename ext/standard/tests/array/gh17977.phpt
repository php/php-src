--TEST--
array_any() / array_all() leak
--DESCRIPTION--
Found in GH-16831#issuecomment-2700410631
--FILE--
<?php

// Don't touch this str_repeat + random_int combination,
// this is to circumvent SCCP and interning
$key = str_repeat('abc', random_int(3, 3));

var_dump(array_any([$key => 1], static fn () => true));
var_dump(array_all([$key => 1], static fn () => false));

?>
--EXPECT--
bool(true)
bool(false)
