--TEST--
SSL cafile stream containing no valid certificates
--EXTENSIONS--
openssl
--SKIPIF--
<?php
if (!function_exists('proc_open')) die('skip no proc_open');
?>
--FILE--
<?php
$serverCode = <<<'CODE'
    $server = stream_socket_server('tcp://127.0.0.1:0', $errno, $errstr);
    phpt_notify_server_start($server);

    $client = stream_socket_accept($server, 2);
    if ($client) {
        fclose($client);
    }
CODE;

$clientCode = <<<'CODE'
    $context = stream_context_create(['ssl' => [
        'cafile' => 'file://%s',
    ]]);
    var_dump(stream_socket_client(
        'ssl://{{ ADDR }}',
        timeout: 2,
        context: $context,
    ));
CODE;
$clientCode = sprintf($clientCode, __DIR__ . '/plain.txt');

include 'ServerClientTestCase.inc';
ServerClientTestCase::getInstance()->run($clientCode, $serverCode);
?>
--EXPECTF--
Warning: stream_socket_client(): no valid certs found cafile stream: '%s' in %sServerClientTestCase.inc(%d) : eval()'d code on line 4

Warning: stream_socket_client(): Failed to enable crypto in %sServerClientTestCase.inc(%d) : eval()'d code on line 4

Warning: stream_socket_client(): Unable to connect to ssl://127.0.0.1:%d (Unknown error) in %sServerClientTestCase.inc(%d) : eval()'d code on line 4
bool(false)
