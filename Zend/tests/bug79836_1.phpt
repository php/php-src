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
--EXPECT--
Done
