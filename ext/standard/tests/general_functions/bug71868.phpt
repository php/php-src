--TEST--
Bug #71868: Environment variables with no value are filtered out by proc_open()
--SKIPIF--
<?php
if (!function_exists('proc_open')) die('skip proc_open() not available');
?>
--FILE--
<?php
$callee = __DIR__ . DIRECTORY_SEPARATOR . "bug71868.inc";
$php = getenv('TEST_PHP_EXECUTABLE');
$cmd = "$php -n $callee";

$descriptors = array();
$pipes = array();

$cwd = __DIR__;
$env = array('FOO' => '');

$process = proc_open($cmd, $descriptors, $pipes, $cwd, $env);
proc_close($process);
?>
--EXPECT--
bool(true)
string(0) ""
