--TEST--
GH-19245: Success error message on TLS stream accept failure
--EXTENSIONS--
openssl
--SKIPIF--
<?php
if (!function_exists("proc_open")) die("skip no proc_open");
?>
--FILE--
<?php
include 'ServerClientTestCase.inc';

$baseDir = __DIR__ . '/gh19245';
$baseDirCertFile = $baseDir . '/cert.crt';
$baseDirPkFile = $baseDir . '/private.key';

$serverCodeTemplate = <<<'CODE'
    ini_set('log_errors', 'On');
    ini_set('open_basedir',  __DIR__ . '/gh19245');
    $serverUri = "ssl://127.0.0.1:0";
    $serverFlags = STREAM_SERVER_BIND | STREAM_SERVER_LISTEN;
    $serverCtx = stream_context_create(['ssl' => [
        'local_cert' => '%s',
        'local_pk' => '%s',
    ]]);

    $sock = stream_socket_server($serverUri, $errno, $errstr, $serverFlags, $serverCtx);
    phpt_notify_server_start($sock);

    $link = stream_socket_accept($sock);
CODE;

$clientCode = <<<'CODE'
    $serverUri = "ssl://{{ ADDR }}";
    $clientFlags = STREAM_CLIENT_CONNECT;

    $clientCtx = stream_context_create(['ssl' => [
        'verify_peer' => false,
        'verify_peer_name' => false
    ]]);

    @stream_socket_client($serverUri, $errno, $errstr, 2, $clientFlags, $clientCtx);
CODE;

$serverCode = sprintf($serverCodeTemplate, $baseDirCertFile . "\0test", $baseDirPkFile);
ServerClientTestCase::getInstance()->run($clientCode, $serverCode);

?>
--EXPECTF--
PHP Warning:  stream_socket_accept(): Path for local_cert in ssl stream context option must not contain any null bytes in %s
PHP Warning:  stream_socket_accept(): Unable to get real path of certificate file `%scert.crt' in %s
PHP Warning:  stream_socket_accept(): Failed to enable crypto in %s
PHP Warning:  stream_socket_accept(): Accept failed: Cannot enable crypto in %s
