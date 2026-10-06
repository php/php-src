--TEST--
Windows CLI INI expansion preserves UTF-8 until the final code page is selected
--SKIPIF--
<?php
if (PHP_OS_FAMILY !== 'Windows') die('skip Windows only');
if (!file_exists(dirname(PHP_BINARY) . '/php_embed_cli_test.exe')) {
    die('skip embed SAPI test host is not built (requires --enable-embed)');
}
?>
--FILE--
<?php
chdir(__DIR__);
$host = escapeshellarg(dirname(PHP_BINARY) . '/php_embed_cli_test.exe');
$prepend = '<?php echo "Prepend works\n";';
file_put_contents("cli_ini_caf\xc3\xa9.php", $prepend);
file_put_contents("cli_ini_caf\xc3\xa9\xc3\xa9.php", $prepend);
file_put_contents('cli_ini_cp932.php', $prepend);
file_put_contents('cli_ini_utf8.php', '<?php
echo sapi_windows_cp_get(), PHP_EOL;
foreach (["auto_prepend_file", "include_path", "user_agent"] as $name) {
    echo bin2hex(ini_get($name)), PHP_EOL;
}');
file_put_contents('cli_ini_utf8.ini', "default_charset=Windows-1252\nprefix=cli_ini_caf\xe9\nsuffix=\xe9\ncaf\xe9=cli_ini_caf\xe9\xe9\ncollision=scalar\n");
putenv("CLI_INI_UTF8=caf\xc3\xa9");
$cases = [
    'ini-expansion' => [
        '-n', '-d', "prefix=cli_ini_caf\xc3\xa9", '-d', "caf\xc3\xa9=cli_ini_caf\xc3\xa9",
        '-d', 'auto_prepend_file=${prefix}.php', '-d', 'include_path=${café}',
        '-d', 'user_agent=${CLI_INI_UTF8}', '-d', 'alias=Windows-1252',
        '-d', 'default_charset=${alias}', '-f', 'cli_ini_utf8.php',
    ],
    'ini-file-expansion' => [
        '-c', 'cli_ini_utf8.ini', '-d', 'auto_prepend_file=${prefix}${suffix}.php',
        '-d', "include_path=cli_ini_caf\xc3\xa9" . '${suffix}', '-d', 'user_agent=${café}',
        '-f', 'cli_ini_utf8.php',
    ],
    'ini-cp932' => [
        '-n', '-d', 'auto_prepend_file=cli_ini_cp932.php',
        '-d', 'include_path=ソ\main.php', '-d', 'token=tail',
        '-d', 'user_agent=ソ${token}', '-d', 'default_charset=CP932', '-f', 'cli_ini_utf8.php',
    ],
    'ini-section' => [
        '-c', 'cli_ini_utf8.ini', '-d', "before=1\n[PATH=collision]\ndefault_charset=UTF-8",
        '-d', 'auto_prepend_file=${prefix}.php', '-d', 'include_path=${prefix}',
        '-d', 'user_agent=${CLI_INI_UTF8}', '-f', 'cli_ini_utf8.php',
    ],
];
foreach ($cases as $mode => $options) {
    passthru("$host $mode", $status);
    var_dump($status);
    $process = proc_open([PHP_BINARY, ...$options], [1 => STDOUT, 2 => STDERR], $pipes);
    var_dump(proc_close($process));
}
?>
--CLEAN--
<?php
unlink(__DIR__ . "/cli_ini_caf\xc3\xa9.php");
unlink(__DIR__ . "/cli_ini_caf\xc3\xa9\xc3\xa9.php");
unlink(__DIR__ . '/cli_ini_cp932.php');
unlink(__DIR__ . '/cli_ini_utf8.php');
unlink(__DIR__ . '/cli_ini_utf8.ini');
?>
--EXPECT--
Prepend works
1252
636c695f696e695f636166e92e706870
636c695f696e695f636166e9
636166e9
int(0)
Prepend works
1252
636c695f696e695f636166e92e706870
636c695f696e695f636166e9
636166e9
int(0)
Prepend works
1252
636c695f696e695f636166e9e92e706870
636c695f696e695f636166e9e9
636c695f696e695f636166e9e9
int(0)
Prepend works
1252
636c695f696e695f636166e9e92e706870
636c695f696e695f636166e9e9
636c695f696e695f636166e9e9
int(0)
Prepend works
932
636c695f696e695f63703933322e706870
835c5c6d61696e2e706870
835c7461696c
int(0)
Prepend works
932
636c695f696e695f63703933322e706870
835c5c6d61696e2e706870
835c7461696c
int(0)
Prepend works
65001
636c695f696e695f636166c3a92e706870
636c695f696e695f636166c3a9
636166c3a9
int(0)
Prepend works
65001
636c695f696e695f636166c3a92e706870
636c695f696e695f636166c3a9
636166c3a9
int(0)
