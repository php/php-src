--TEST--
proc_open with bypass_shell
--SKIPIF--
<?php # vim:syn=php
if (!is_executable("/bin/cat")) echo "skip";
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
		array("/bin/cat"),
		$ds,
		$pipes,
		NULL,
		NULL,
		array('bypass_shell' => TRUE)
		);

proc_close($cat);

echo "I didn't segfault!\n";

?>
--EXPECT--
I didn't segfault!
