--TEST--
Test function pcntl_rfork() with wrong flags
--EXTENSIONS--
pcntl
posix
--SKIPIF--
<?php
	if (!function_exists('pcntl_rfork')) die('skip pcntl_rfork unavailable');
?>
--FILE--
<?php
echo "\n*** Test with RFMEM ***\n";
try {
	$pid = pcntl_rfork(32);
} catch (ValueError $e) {
	echo $e;
}
echo "\n*** Test with RFSIGSHARE ***\n";
try {
	$pid = pcntl_rfork(16384);
} catch (ValueError $e) {
	echo $e;
}
echo "\n*** Test with RFFDG|RFCFDG ***\n";
try {
	$pid = pcntl_rfork(RFFDG|RFCFDG);
} catch (ValueError $e) {
	echo $e;
}
?>
--EXPECTF--
*** Test with RFMEM ***
ValueError: pcntl_rfork(): Argument #1 ($flags) must not include RFMEM value, not allowed within this context in %s
Stack trace:
#0 %s: pcntl_rfork(32)
#1 {main}
*** Test with RFSIGSHARE ***
ValueError: pcntl_rfork(): Argument #1 ($flags) must not include RFSIGSHARE value, not allowed within this context in %s
Stack trace:
#0 %s: pcntl_rfork(16384)
#1 {main}
*** Test with RFFDG|RFCFDG ***
ValueError: pcntl_rfork(): Argument #1 ($flags) must not include both RFFDG and RFCFDG, because these flags are mutually exclusive in %s
Stack trace:
#0 %s: pcntl_rfork(4100)
#1 {main}
