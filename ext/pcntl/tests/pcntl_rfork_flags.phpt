--TEST--
Test function pcntl_rfork() with RFCFDG and RFFDG flags.
--SKIPIF--
<?php
	if (!extension_loaded('pcntl')) die('skip pcntl extension not available');
	elseif (!extension_loaded('posix')) die('skip posix extension not available');
  if (!function_exists('pcntl_rfork')) die('skip pcntl_rfork unavailable');
?>
--FILE--
<?php
echo "\n*** Test with RFFDG and RFCFDG flags ***\n";
$pid = pcntl_rfork(RFFDG);
if ($pid > 0) {
	pcntl_wait($status);
  var_dump($pid);
} else {
	var_dump($pid);
  exit;
}

$pid = pcntl_rfork(RFCFDG);
if ($pid > 0) {
  pcntl_wait($status);
  var_dump($pid);
}
?>
--EXPECTF--
*** Test with RFFDG and RFCFDG flags ***
int(0)
int(%d)
int(%d)
