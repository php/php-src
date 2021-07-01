--TEST--
Test function pcntl_rfork() with no flag.
--EXTENSIONS--
pcntl
posix
--SKIPIF--
<?php
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
--EXPECT--
*** Test with no flags ***
child
