--TEST--
proc_open with bypass_shell and environment
--SKIPIF--
<?php # vim:syn=php
if (!is_executable("/usr/bin/env")) echo "skip";
if (!function_exists("proc_open")) echo "skip proc_open() is not available";
if( substr(PHP_OS, 0, 3) == 'WIN' ) die("skip this test for non-Windows systems only ");
?>
--FILE--
<?php
$ds = array(
		0 => array("pipe", "r"),
		1 => array("pipe", "w"),
		2 => array("pipe", "w")
		);

$cat = proc_open(
		array("/usr/bin/env", "env"),
		$ds,
		$pipes,
		NULL,
		array(b"TEST_ENV" => 42, b"TEST_ENV_2" => 84),
		array('bypass_shell' => TRUE)
		);

echo stream_get_contents($pipes[1]);

proc_close($cat);

// %A is put under EXPECTF as Valgrind will append extra environment
?>
--EXPECTF--
TEST_ENV=42
TEST_ENV_2=84%A
