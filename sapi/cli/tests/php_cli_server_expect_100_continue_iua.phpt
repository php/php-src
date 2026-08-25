--TEST--
Failure to send "100 Continue" is reported with ignore_user_abort=1
--SKIPIF--
<?php
include "skipif.inc";
if (!extension_loaded("sockets")) die("skip sockets extension required");
if (PHP_OS_FAMILY === "Windows") die("skip SO_LINGER reset behaviour differs on Windows");
?>
--FILE--
<?php
include "php_cli_server.inc";
$server = php_cli_server_start('echo "Hello world";', 'index.php', ['-d', 'ignore_user_abort=1']);

$fp = fsockopen(PHP_CLI_SERVER_HOSTNAME, PHP_CLI_SERVER_PORT);
socket_set_option(socket_import_stream($fp), SOL_SOCKET, SO_LINGER, ['l_onoff' => 1, 'l_linger' => 0]);
fwrite($fp, "POST / HTTP/1.1\r\nExpect: 100-continue\r\nContent-Length: 4\r\n\r\n");
fclose($fp);

$output = '';
for ($i = 0; $i < 100 && !str_contains($output, 'Invalid request'); $i++) {
    usleep(50000);
    $output = file_get_contents($server->outputFile);
}

var_dump(str_contains($output, 'Invalid request'), str_contains($output, 'Unexpected EOF'));
?>
--CLEAN--
<?php
@unlink(__DIR__ . '/php_cli_server_expect_100_continue_iua.log')
?>
--EXPECT--
bool(true)
bool(false)
