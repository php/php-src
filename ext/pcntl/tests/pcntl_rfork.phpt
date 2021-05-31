--TEST--
Test function pcntl_rfork() with no flag.
--SKIPIF--
<?php
	if (!extension_loaded('pcntl')) die('skip pcntl extension not available');
	elseif (!extension_loaded('posix')) die('skip posix extension not available');
  if (!function_exists('pcntl_rfork')) die('skip pcntl_rfork unavailable');
?>
--FILE--
<?php
echo "*** Test with no flags ***\n";

$pid = pcntl_rfork(0);
if ($pid == 0) {
	echo "child";
  exit;
}
?>
--EXPECTF--
*** Test with no flags ***
child
