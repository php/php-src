--TEST--
shell_exec() with exit code setting
--SKIPIF--
<?php
if (!is_executable("/bin/true")) echo "skip";
if (!function_exists("shell_exec")) echo "skip shell_exec() is not available";
?>
--FILE--
<?php
$retcode = null;

$commands = [
	"/bin/true",
	"non-existent-command > /dev/null 2>&1"
];

foreach ($commands as $command) {
	shell_exec($command, $retcode);
	var_dump($retcode);
}

echo "Done\n";
?>
--EXPECT--
int(0)
int(127)
Done
