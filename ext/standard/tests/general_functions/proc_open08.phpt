--TEST--
proc_open with only one argv
--DESCRIPTION--
This test tries out a very esoteric functionality: Passing no argv[0] to a
program. There's absolutely no reason anyone would do this in practice, but the
entire point of the bypass_shell patch was to allow 100% control over the child
process, so the option is there. Keep in mind that actually using this
"feature" will probably crash most programs one could run, since the expression
argc > 0 in a main() function is pretty much guaranteed by POSIX. It's
interesting to note that PHP itself handles this case gracefully.
--SKIPIF--
<?php # vim:syn=php
if (!is_executable($_ENV['TEST_PHP_EXECUTABLE'])) echo "skip";
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
		array($_ENV['TEST_PHP_EXECUTABLE']),
		$ds,
		$pipes,
		NULL,
		NULL,
		array('bypass_shell' => TRUE)
		);

fprintf($pipes[0], '<?php error_reporting(E_ALL); var_dump($argv); ?>');
fclose($pipes[0]);

echo stream_get_contents($pipes[1]);

proc_close($cat);

?>
--EXPECTF--
Notice: Undefined variable: argv in %s- on line 1
NULL
