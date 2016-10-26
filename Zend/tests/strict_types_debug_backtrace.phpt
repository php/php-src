--TEST--
Strict types reported in debug backtrace
--FILE--
<?php declare(strict_types=0);

require 'strict_types_debug_backtrace_helper.inc';

function test_strict_types_debug_backtrace1()
{
	return debug_backtrace();
}

$bt1 = test_strict_types_debug_backtrace1();
$bt2 = test_strict_types_debug_backtrace2();

var_dump($bt1[0]['strict_types']);
var_dump($bt2[0]['strict_types'], $bt2[1]['strict_types']);

echo "Done\n";
?>
--EXPECTF--	
int(0)
int(0)
int(1)
Done
