--TEST--
INI value from env var with default [2]
--FILE--
<?php
$ini_file = __DIR__ . DIRECTORY_SEPARATOR . 'test.ini';
$cwd = __DIR__;
$php = '"'.getenv('TEST_PHP_EXECUTABLE').'"';
$code = '<?php var_dump(ini_get("disable_classes")); ?>';
$env = [
	// 'PHP_DISABLE_CLASSES' => 'NotARealClass'
];

$process = proc_open(
	$php . ' -c "' . $ini_file . '"',
	[ [ 'pipe', 'r' ], [ 'pipe', 'w' ], [ 'pipe', 'w' ] ],
	$pipes,
	$cwd,
	$env
);

if (is_resource($process)) {
	fwrite($pipes[0], $code);
	fclose($pipes[0]);

	echo stream_get_contents($pipes[1]);
	fclose($pipes[1]);

	$return_value = proc_close($process);
}
?>
--EXPECT--
string(13) "FallbackClass"
