--TEST--
Windows CLI and embed argv use the configured code page after UTF-8 option parsing
--SKIPIF--
<?php
if (PHP_OS_FAMILY !== 'Windows') die('skip Windows only');
if (!file_exists(dirname(PHP_BINARY) . '/php_embed_cli_test.exe')) {
    die('skip embed SAPI test host is not built (requires --enable-embed)');
}
?>
--FILE--
<?php
$host = dirname(PHP_BINARY) . '/php_embed_cli_test.exe';
function run_cli(array $command, bool $invalid_utf8 = false): void {
    $env = $invalid_utf8 ? ['PHP_EMBED_TEST_INVALID_UTF8' => '1'] + getenv() : null;
    $process = proc_open($command, [1 => STDOUT, 2 => STDOUT], $pipes, null, $env);
    var_dump(proc_close($process));
}
$code = <<<'PHP'
echo json_encode([$argc, array_map('bin2hex', array_slice($argv, 1))]), PHP_EOL;
echo sapi_windows_cp_get(), PHP_EOL;
exit(23);
PHP;
foreach ([
    ['default_charset=UTF-8', "\xf0\x9f\x98\x80"],
    ['default_charset=Windows-1252', "\xe2\x82\xac"],
    ['internal_encoding=Windows-1252', "\xe2\x82\xac"],
] as [$setting, $last_arg]) {
    $options = ['-n', '-d', $setting, '-r', $code, '--', "caf\xc3\xa9", 'argument with spaces', '', $last_arg];
    run_cli([$host, '--', ...$options]);
    run_cli([$host, '--', ...$options], true);
}
run_cli([$host, '--', '-n', '-d', 'default_charset=Windows-1252', '-d', 'internal_encoding=',
    '-d', "user_agent=caf\xc3\xa9", '-r',
    "echo sapi_windows_cp_get(), PHP_EOL; echo bin2hex(ini_get('user_agent')), PHP_EOL;"]);
chdir(__DIR__);
file_put_contents("cli_argv_caf\xc3\xa9.php", '<?php echo "Unicode filename works\n";');
run_cli([$host, '--', '-n', '-d', 'default_charset=Windows-1252', '-f', "cli_argv_caf\xc3\xa9.php"]);
file_put_contents('cli_argv_main.php', '<?php echo "Main script works\n"; echo sapi_windows_cp_get(), PHP_EOL; echo bin2hex(ini_get("auto_prepend_file")), PHP_EOL;');
$prepend = "auto_prepend_file=cli_argv_caf\xc3\xa9.php";
run_cli([$host, '--', '-n', '-d', $prepend, '-d', 'default_charset=Windows-1252', '-f', 'cli_argv_main.php']);
file_put_contents('cli_argv_test.ini', "default_charset=Windows-1252\n");
run_cli([$host, '--', '-c', 'cli_argv_test.ini', '-d', $prepend, '-f', 'cli_argv_main.php']);
run_cli([PHP_BINARY, '-n', '-d', $prepend, '-f', 'cli_argv_main.php']);
run_cli([PHP_BINARY, '-n', '-d', 'default_charset=Windows-1252', '-d', $prepend, '-f', 'cli_argv_main.php']);
run_cli([PHP_BINARY, '-n', '-d', 'default_charset=Windows-1252', '-d', 'internal_encoding=',
    '-d', $prepend, '-f', 'cli_argv_main.php']);
?>
--CLEAN--
<?php
unlink(__DIR__ . "/cli_argv_caf\xc3\xa9.php");
unlink(__DIR__ . '/cli_argv_main.php');
unlink(__DIR__ . '/cli_argv_test.ini');
?>
--EXPECT--
[5,["636166c3a9","617267756d656e74207769746820737061636573","","f09f9880"]]
65001
int(23)
Invalid UTF-8 in command line argument 10.
int(1)
[5,["636166e9","617267756d656e74207769746820737061636573","","80"]]
1252
int(23)
Invalid UTF-8 in command line argument 10.
int(1)
[5,["636166e9","617267756d656e74207769746820737061636573","","80"]]
1252
int(23)
Invalid UTF-8 in command line argument 10.
int(1)
1252
636166e9
int(0)
Unicode filename works
int(0)
Unicode filename works
Main script works
1252
636c695f617267765f636166e92e706870
int(0)
Unicode filename works
Main script works
1252
636c695f617267765f636166e92e706870
int(0)
Unicode filename works
Main script works
65001
636c695f617267765f636166c3a92e706870
int(0)
Unicode filename works
Main script works
1252
636c695f617267765f636166e92e706870
int(0)
Unicode filename works
Main script works
1252
636c695f617267765f636166e92e706870
int(0)
