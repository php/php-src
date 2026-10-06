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
foreach (['ignored-host-argument', 'default-charset', 'internal-encoding', 'empty-internal-encoding'] as $mode) {
    passthru(escapeshellarg($host) . ' ' . $mode, $status);
    var_dump($status);
    passthru(escapeshellarg($host) . ' ' . $mode . ' invalid-utf8 2>&1', $status);
    var_dump($status);
}
chdir(__DIR__);
file_put_contents("cli_argv_caf\xc3\xa9.php", '<?php echo "Unicode filename works\n";');
passthru(escapeshellarg($host) . ' default-charset file', $status);
var_dump($status);
file_put_contents('cli_argv_main.php', '<?php echo "Main script works\n"; echo sapi_windows_cp_get(), PHP_EOL; echo bin2hex(ini_get("auto_prepend_file")), PHP_EOL;');
passthru(escapeshellarg($host) . ' prepend', $status);
var_dump($status);
file_put_contents('cli_argv_test.ini', "default_charset=Windows-1252\n");
passthru(escapeshellarg($host) . ' prepend-ini', $status);
var_dump($status);
$native = escapeshellarg(PHP_BINARY);
$prepend = escapeshellarg("auto_prepend_file=cli_argv_caf\xc3\xa9.php");
passthru("$native -n -d $prepend -f cli_argv_main.php", $status);
var_dump($status);
passthru("$native -n -d default_charset=Windows-1252 -d $prepend -f cli_argv_main.php", $status);
var_dump($status);
passthru("$native -n -d default_charset=Windows-1252 -d internal_encoding= -d $prepend -f cli_argv_main.php", $status);
var_dump($status);
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
Invalid UTF-8 in command line argument 7.
int(1)
[5,["636166e9","617267756d656e74207769746820737061636573","","80"]]
1252
int(23)
Invalid UTF-8 in command line argument 7.
int(1)
[5,["636166e9","617267756d656e74207769746820737061636573","","80"]]
1252
int(23)
Invalid UTF-8 in command line argument 7.
int(1)
[5,["636166e9","617267756d656e74207769746820737061636573","","80"]]
1252
int(23)
Invalid UTF-8 in command line argument 7.
int(1)
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
