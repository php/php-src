--TEST--
Bug #79836 (Segfault in concat_function)
--INI--
opcache.optimization_level = 0x7FFEBFFF & ~0x400
--FILE--
<?php
$counter = 0;
ob_start(function ($buffer) use (&$c, &$counter) {
	$c = 0;
	++$counter;
}, 1);
$c .= [];
$c .= [];
ob_end_clean();
echo $counter . "\n";
?>
--EXPECTF--
Deprecated: main(): Returning a non-string result from user output handler Closure::__invoke is deprecated in %s on line %d

Deprecated: main(): Returning a non-string result from user output handler Closure::__invoke is deprecated in %s on line %d

Deprecated: ob_end_clean(): Returning a non-string result from user output handler Closure::__invoke is deprecated in %s on line %d
3
