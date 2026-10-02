--TEST--
do_php_cli() validates and converts custom UTF-8 argv on Windows
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
foreach (['ignored-host-argument', 'default-charset', 'internal-encoding'] as $mode) {
    passthru(escapeshellarg($host) . ' ' . $mode, $status);
    var_dump($status);
    passthru(escapeshellarg($host) . ' ' . $mode . ' invalid-utf8 2>&1', $status);
    var_dump($status);
}
chdir(__DIR__);
file_put_contents("cli_argv_caf\xc3\xa9.php", '<?php echo "Unicode filename works\n";');
passthru(escapeshellarg($host) . ' default-charset file', $status);
var_dump($status);
?>
--CLEAN--
<?php
unlink(__DIR__ . "/cli_argv_caf\xc3\xa9.php");
?>
--EXPECT--
[5,["636166c3a9","617267756d656e74207769746820737061636573","","f09f9880"]]
int(23)
Invalid UTF-8 in command line argument 7.
int(1)
[5,["636166e9","617267756d656e74207769746820737061636573","","80"]]
int(23)
Invalid UTF-8 in command line argument 7.
int(1)
[5,["636166e9","617267756d656e74207769746820737061636573","","80"]]
int(23)
Invalid UTF-8 in command line argument 7.
int(1)
Unicode filename works
int(0)
