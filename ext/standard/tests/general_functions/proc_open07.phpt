--TEST--
proc_open with no argv
--SKIPIF--
<?php # vim:syn=php
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
		array(),
		$ds,
		$pipes,
		NULL,
		array("TEST_ENV" => 42, "TEST_ENV_2" => 84),
		array('bypass_shell' => TRUE)
		);

var_dump($cat);

?>
--EXPECTF--

Warning: proc_open(): arguments array must have at least one element in %s/proc_open07.php on line %d
bool(false)
