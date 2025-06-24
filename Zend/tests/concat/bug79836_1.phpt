--TEST--
Bug #79836 (Segfault in concat_function)
--INI--
opcache.optimization_level = 0x7FFEBFFF & ~0x400
--FILE--
<?php
$x = 'non-empty';
ob_start(function () use (&$c) {
	$c = 0;
}, 1);
$c = [];
$x = $c . $x;
$x = $c . $x;
ob_end_clean();
echo "Done\n";
?>
--EXPECTF--
Deprecated: main(): Returning a non-string result from user output handler Closure::__invoke is deprecated in %s on line %d

Deprecated: ob_end_clean(): Returning a non-string result from user output handler Closure::__invoke is deprecated in %s on line %d
Done
