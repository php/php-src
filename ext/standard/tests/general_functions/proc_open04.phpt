--TEST--
proc_open with bypass_shell
--SKIPIF--
<?php # vim:syn=php
if (!is_executable("/bin/echo")) echo "skip";
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
		array("/bin/echo", "echo", "asdf"),
		$ds,
		$pipes,
		NULL,
		NULL,
		array('bypass_shell' => TRUE)
		);

echo stream_get_contents($pipes[1]);

proc_close($cat);

?>
--EXPECT--
asdf
