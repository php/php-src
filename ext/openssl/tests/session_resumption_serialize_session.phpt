--TEST--
TLS session resumption - serialize session
--EXTENSIONS--
openssl
--SKIPIF--
<?php
if (!function_exists("proc_open")) die("skip no proc_open");
?>
--FILE--
<?php
$certFile = __DIR__ . DIRECTORY_SEPARATOR . 'session_resumption_serialize.pem.tmp';

$serverCode = <<<'CODE'
    $flags = STREAM_SERVER_BIND|STREAM_SERVER_LISTEN;
    $ctx = stream_context_create(['ssl' => [
        'local_cert' => '%s',
        'session_cache' => true,
        'session_id_context' => 'test-basic',
    ]]);

    $server = stream_socket_server('tls://127.0.0.1:0', $errno, $errstr, $flags, $ctx);
    phpt_notify_server_start($server);

    /* Accept single connections */
    $client = @stream_socket_accept($server, 30);
    if ($client) {
        fwrite($client, "Hello from server\n");
        fclose($client);
    }
CODE;
$serverCode = sprintf($serverCode, $certFile);

$clientCode = <<<'CODE'
    $sessionData = '';

    $flags = STREAM_CLIENT_CONNECT;
    $ctx = stream_context_create(['ssl' => [
        'verify_peer' => false,
        'verify_peer_name' => false,
        'session_new_cb' => function($stream, $session) use (&$sessionData) {
            if (empty($sessionData)) {
                $serializedSessionData = serialize($session);
                var_dump($serializedSessionData);
                $session = unserialize($serializedSessionData);
                var_dump($session);
            }
            $sessionData = $session;
        }
    ]]);

    /* First connection - full handshake */
    $client1 = stream_socket_client("tls://{{ ADDR }}", $errno, $errstr, 30, $flags, $ctx);
    if ($client1) {
        echo trim(fgets($client1)) . "\n";
        $meta1 = stream_get_meta_data($client1);
        echo "First connection resumed: " . ($meta1['crypto']['session_reused'] ? "yes" : "no") . "\n";
        echo "Session data received: " . (!empty($sessionData) ? "yes" : "no") . "\n";
        fclose($client1);
    }
CODE;

include 'CertificateGenerator.inc';
$certificateGenerator = new CertificateGenerator();
$certificateGenerator->saveNewCertAsFileWithKey('session_resumption_test', $certFile);

include 'ServerClientTestCase.inc';
ServerClientTestCase::getInstance()->run($clientCode, $serverCode);
?>
--CLEAN--
<?php
@unlink(__DIR__ . DIRECTORY_SEPARATOR . 'session_resumption_serialize.pem.tmp');
?>
--EXPECTF--
string(%d) "O:15:"Openssl\Session":1:{s:3:"pem";s:%d:"-----BEGIN SSL SESSION PARAMETERS-----
%a
-----END SSL SESSION PARAMETERS-----
";}"
object(Openssl\Session)#9 (1) {
  ["id"]=>
%a
}
Hello from server
First connection resumed: no
Session data received: yes
