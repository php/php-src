--TEST--
Bug #43450 (Memory leak on some functions with implicit object __toString() call)
--SKIPIF--
<?php if (!function_exists('memory_get_usage')) die('memory_get_usage() not installed'); ?>
--FILE--
<?php
error_reporting(E_ALL|E_STRICT);

class Foo 
{
    public function __toString()
	{
	    return __CLASS__;
	}
}

$num_repeats = 100000;

$start = (memory_get_usage() / 1024) + 16;
for ($i=1;$i<$num_repeats;$i++) 
{
	$foo = new Foo();
	md5($foo);
}
$end = memory_get_peak_usage() / 1024;

if ($start < $end) {
	echo 'FAIL';
} else {
	echo 'PASS';
}

?>
--EXPECT--
PASS
