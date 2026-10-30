--TEST--
Closing the stream in the `session_get_cb` callback triggers use after free
--EXTENSIONS--
openssl
--CREDITS--
Calif.io
--CLEAN--
<?php

$certFile = sys_get_temp_dir() . '/php-audit-session-get-' . getmypid() . '.pem';
@unlink($certFile);
?>
--FILE--
<?php

$certFile = sys_get_temp_dir() . '/php-audit-session-get-' . getmypid() . '.pem';

$serverCode = <<<'PHP'
$sessions = [];
$context = stream_context_create(['ssl' => [
    'local_cert' => '%s',
    'crypto_method' => STREAM_CRYPTO_METHOD_TLSv1_2_SERVER,
    'session_id_context' => 'audit-session-get',
    'session_new_cb' => static function ($stream, $session) use (&$sessions): void {
        $sessions[bin2hex($session->id)] = $session;
    },
    'session_get_cb' => static function ($stream, string $sessionId) use (&$sessions) {
        $session = $sessions[bin2hex($sessionId)] ?? null;
        fclose($stream);

        return $session;
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

for ($i = 0; $i < 2; $i++) {
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
    'crypto_method' => STREAM_CRYPTO_METHOD_TLSv1_2_CLIENT,
    'verify_peer' => false,
    'verify_peer_name' => false,
    'session_new_cb' => static function ($stream, $newSession) use (&$session): void {
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
if ($first) {
    @fgets($first);
    fclose($first);
}

$resumeContext = stream_context_create(['ssl' => [
    'crypto_method' => STREAM_CRYPTO_METHOD_TLSv1_2_CLIENT,
    'verify_peer' => false,
    'verify_peer_name' => false,
    'session_data' => $session,
]]);

@stream_socket_client(
    'tls://{{ ADDR }}',
    $errno,
    $errstr,
    10,
    STREAM_CLIENT_CONNECT,
    $resumeContext,
);
PHP;

require 'CertificateGenerator.inc';
$certificateGenerator = new CertificateGenerator();
$certificateGenerator->saveNewCertAsFileWithKey('audit_session_get', $certFile);

require 'ServerClientTestCase.inc';
ServerClientTestCase::getInstance()->run($clientCode, $serverCode);

?>
--EXPECT--
