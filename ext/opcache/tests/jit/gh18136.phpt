--TEST--
GH-18136 (tracing JIT floating point register clobbering on Windows and ARM64)
--EXTENSIONS--
opcache
--INI--
opcache.jit=tracing
opcache.jit_buffer_size=64M
opcache.jit_hot_func=4
opcache.jit_hot_loop=4
--FILE--
<?php
namespace Foo;

function diff($point1, $point2)
{
	$a = deg2rad($point1); // Prefixing these with \ also makes the issue go away
	$b = deg2rad($point2);
	return $a - $b;
}

function getRawDistance()
{
	$distance = 0;
	for ($p = 0; $p < 200; $p++) {
		// Needs to be a namespaced call_user_func call to reproduce the issue (i.e. adding \ at front makes the issue go away)
		$distance += call_user_func('Foo\diff', 0, $p);
	}

	return $distance;
}

var_dump(getRawDistance());
?>
--EXPECT--
float(-347.3205211468715)
