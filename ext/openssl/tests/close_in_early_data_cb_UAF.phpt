--TEST--
Closing the stream in the `early_data_cb` callback triggers use after free
--EXTENSIONS--
openssl
--CREDITS--
Calif.io
--CLEAN--
<?php

$certFile = sys_get_temp_dir() . '/php-audit-early-data-' . getmypid() . '.pem';
@unlink($certFile);
?>
--FILE--
<?php

$certFile = sys_get_temp_dir() . '/php-audit-early-data-' . getmypid() . '.pem';

$serverCode = <<<'PHP'
$context = stream_context_create(['ssl' => [
    'local_cert' => '%s',
    'session_id_context' => 'audit-early-data',
    'max_early_data' => 16384,
    'early_data_cb' => function ($stream, string $data): void {
        fclose($stream);
    },
]]);

$server = stream_socket_server(
    'tls://127.0.0.1:0',
    $errno,
    $errstr,
    STREAM_SERVER_BIND | STREAM_SERVER_LISTEN,
    $context,
);
phpt_notify_server_start($server);

for ($connection = 0; $connection < 2; $connection++) {
    $client = @stream_socket_accept($server, 10);
    if ($client) {
        fwrite($client, "response\n");
        fclose($client);
    }
}
PHP;
$serverCode = sprintf($serverCode, $certFile);

$clientCode = <<<'PHP'
$session = null;
$context = stream_context_create(['ssl' => [
    'verify_peer' => false,
    'verify_peer_name' => false,
    'session_new_cb' => function ($stream, $newSession) use (&$session): void {
        $session = $newSession;
    },
]]);

$first = stream_socket_client(
    'tls://{{ ADDR }}',
    $errno,
    $errstr,
    10,
    STREAM_CLIENT_CONNECT,
    $context,
);
fgets($first);
fclose($first);

$earlyDataContext = stream_context_create(['ssl' => [
    'verify_peer' => false,
    'verify_peer_name' => false,
    'session_data' => $session,
    'early_data' => 'attacker-controlled-early-data',
]]);

@stream_socket_client(
    'tls://{{ ADDR }}',
    $errno,
    $errstr,
    10,
    STREAM_CLIENT_CONNECT,
    $earlyDataContext,
);
PHP;

require 'CertificateGenerator.inc';
$certificateGenerator = new CertificateGenerator();
$certificateGenerator->saveNewCertAsFileWithKey('audit_early_data', $certFile);

require 'ServerClientTestCase.inc';
ServerClientTestCase::getInstance()->run($clientCode, $serverCode);

?>
--EXPECT--
