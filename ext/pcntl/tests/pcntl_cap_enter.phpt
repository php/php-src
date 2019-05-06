--TEST--
Test function pcntl_cap_enter()
--CREDITS--
David Carlier devnexen@gmail.com
--SKIPIF--
<?php
	if (!extension_loaded('pcntl')) die('skip pcntl extension not available');
	if (!function_exists('pcntl_cap_enter')) die('skip pcntl_cap_enter test');
?>
--FILE--
<?php
echo "*** Test from parent and child process perspective ***\n";

var_dump(pcntl_cap_enabled());
var_dump(pcntl_cap_enter());
var_dump(pcntl_cap_enter());
var_dump(pcntl_cap_enabled());
$pid = pcntl_fork();
if ($pid > 0) {
	pcntl_wait($status);
} else {
	var_dump($pid);
	var_dump(pcntl_cap_enter());
	var_dump(pcntl_cap_enabled());
}
?>
--EXPECTF--
*** Test from parent and child process perspective ***
bool(false)
bool(true)
bool(true)
bool(true)
int(%d)
bool(true)
bool(true)
