--TEST--
proc_open with bypass_shell, environment, and non-string arguments
--SKIPIF--
<?php # vim:syn=php
if (!is_executable("/bin/echo")) echo "skip";
if (!function_exists("proc_open")) echo "skip proc_open() is not available";
if (substr(PHP_OS, 0, 3) == 'WIN') echo "skip this test for non-Windows systems only";
?>
--FILE--
<?php
$ds = array(
		0 => array("pipe", "r"),
		1 => array("pipe", "w"),
		2 => array("pipe", "w")
		);

$cat = proc_open(
		array("/bin/echo", "echo", 1, 2, 3, 4, 5),
		$ds,
		$pipes,
		NULL,
		array("TEST_ENV" => 42, "TEST_ENV_2" => 84),
		array('bypass_shell' => TRUE)
		);

echo stream_get_contents($pipes[1]);

proc_close($cat);

?>
--EXPECTF--
1 2 3 4 5
