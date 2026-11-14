--TEST--
proc_open() accepts a Windows environment block larger than 32760 characters
--SKIPIF--
<?php
if (PHP_OS_FAMILY !== 'Windows') die('skip Windows only');
if (!function_exists('proc_open')) die('skip proc_open() is not available');
?>
--FILE--
<?php
$env = [
    'PATH' => getenv('PATH'),
    'SystemRoot' => getenv('SystemRoot'),
];
for ($index = 0; $index < 8; $index++) {
    $env["PHP_TEST_ENV_$index"] = str_repeat(chr(65 + $index), 5000);
}

$process = proc_open(
    [PHP_BINARY, '-n', '-r', 'echo strlen(getenv("PHP_TEST_ENV_0")), " ", strlen(getenv("PHP_TEST_ENV_7")), "\n";'],
    [1 => ['pipe', 'w'], 2 => ['pipe', 'w']],
    $pipes,
    null,
    $env
);
echo stream_get_contents($pipes[1]);
fclose($pipes[1]);
fclose($pipes[2]);
var_dump(proc_close($process));
?>
--EXPECT--
5000 5000
int(0)
