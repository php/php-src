--TEST--
GH-10801 (Named arguments in CTE functions cause a segfault)
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.optimization_level=0xe0
--EXTENSIONS--
opcache
--FILE--
<?php
// Named argument case with CTE
print_r(array_keys(array: [1 => 1], strict: true, filter_value: 0));
// Will not use named arguments, and must result in the same output
print_r(array_keys(array: [1 => 1], filter_value: 0, strict: true));
?>
--EXPECT--
Array
(
)
Array
(
)
