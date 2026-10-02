--TEST--
Closing the stream in the `session_new_cb` callback triggers use after free
--EXTENSIONS--
openssl
--CREDITS--
Calif.io
--CLEAN--
<?php

$certFile = sys_get_temp_dir() . '/php-audit-session-' . getmypid() . '.pem';
@unlink($certFile);
?>
--FILE--
<?php

$certFile = sys_get_temp_dir() . '/php-audit-session-' . getmypid() . '.pem';

$serverCode = <<<'PHP'
$context = stream_context_create(['ssl' => [
    'local_cert' => '%s',
    'session_cache' => true,
    'session_id_context' => 'audit-session',
]]);

$server = stream_socket_server(
    'tls://127.0.0.1:0',
    $errno,
    $errstr,
    STREAM_SERVER_BIND | STREAM_SERVER_LISTEN,
    $context,
);
phpt_notify_server_start($server);
$client = @stream_socket_accept($server, 10);
if ($client) {
    fwrite($client, "ticket\n");
    fclose($client);
}
PHP;
$serverCode = sprintf($serverCode, $certFile);

$clientCode = <<<'PHP'
$context = stream_context_create(['ssl' => [
    'verify_peer' => false,
    'verify_peer_name' => false,
    'session_new_cb' => function ($stream, $session): void {
        fclose($stream);
    },
]]);

$client = @stream_socket_client(
    'tls://{{ ADDR }}',
    $errno,
    $errstr,
    10,
    STREAM_CLIENT_CONNECT,
    $context,
);
if ($client) {
    @fgets($client);
}
PHP;

require 'CertificateGenerator.inc';
$certificateGenerator = new CertificateGenerator();
$certificateGenerator->saveNewCertAsFileWithKey('audit_session', $certFile);

require 'ServerClientTestCase.inc';
ServerClientTestCase::getInstance()->run($clientCode, $serverCode);

?>
--EXPECT--
