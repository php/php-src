--TEST--
TLS server rate-limits client-initiated renegotiation
--EXTENSIONS--
openssl
--SKIPIF--
<?php
if (!function_exists("proc_open")) die("skip no proc_open");
exec('openssl help', $out, $code);
if ($code > 0) die("skip couldn't locate openssl binary");
if(substr(PHP_OS, 0, 3) == 'WIN') {
    die('skip not suitable for Windows');
}
?>
--FILE--
<?php
$certFile = __DIR__ . DIRECTORY_SEPARATOR . 'stream_server_reneg_limit.pem.tmp';

/**
 * This test uses the openssl binary directly to initiate renegotiation. At this time it's not
 * possible renegotiate the TLS handshake in PHP userland, so using the openssl s_client binary
 * command is the only feasible way to test renegotiation limiting functionality. It's not an ideal
 * solution, but it's really the only way to get test coverage on the rate-limiting functionality
 * given current limitations.
 */

$serverCode = <<<'CODE'
    $printed = false;
    $serverUri = "ssl://127.0.0.1:64321";
    $serverFlags = STREAM_SERVER_BIND | STREAM_SERVER_LISTEN;
    $serverCtx = stream_context_create(['ssl' => [
        'local_cert' => '%s',
        // TLS 1.3 does not support renegotiation.
        'max_proto_version' => STREAM_CRYPTO_PROTO_TLSv1_2,
        'reneg_limit' => 0,
        'reneg_window' => 30,
        'reneg_limit_callback' => function($stream) use (&$printed) {
            if (!$printed) {
                $printed = true;
                var_dump($stream);
            }
        }
    ]]);

    $server = stream_socket_server($serverUri, $errno, $errstr, $serverFlags, $serverCtx);
    phpt_notify();

    $clients = [];
    while (1) {
        $r = array_merge([$server], $clients);
        $w = $e = [];

        stream_select($r, $w, $e, $timeout=42);

        foreach ($r as $sock) {
            if ($sock === $server && ($client = stream_socket_accept($server, $timeout = 42))) {
                $clientId = (int) $client;
                $clients[$clientId] = $client;
            } elseif ($sock !== $server) {
                $clientId = (int) $sock;
                $buffer = fread($sock, 1024);
                if (strlen($buffer)) {
                    continue;
                } elseif (!is_resource($sock) || feof($sock)) {
                    unset($clients[$clientId]);
                    break 2;
                }
            }
        }
    }
CODE;
$serverCode = sprintf($serverCode, $certFile);

$clientCode = <<<'CODE'
    phpt_wait();

    $cmd = 'openssl s_client -connect 127.0.0.1:64321';
    $descriptorSpec = [["pipe", "r"], ["pipe", "w"], ["pipe", "w"]];
    $process = proc_open($cmd, $descriptorSpec, $pipes);

    list($stdin, $stdout, $stderr) = $pipes;

    // Trigger renegotiation twice
    // Server settings only allow one per second (should result in disconnection)
    fwrite($stdin, "R\nR\nR\nR\n");

    $lines = [];
    while(!feof($stderr)) {
        fgets($stderr);
    }

    fclose($stdin);
    fclose($stdout);
    fclose($stderr);
    proc_terminate($process);
CODE;

include 'CertificateGenerator.inc';
$certificateGenerator = new CertificateGenerator();
$certificateGenerator->saveNewCertAsFileWithKey('stream_security_level', $certFile);

include 'ServerClientTestCase.inc';
ServerClientTestCase::getInstance()->run($serverCode, $clientCode);
?>
--CLEAN--
<?php
@unlink(__DIR__ . DIRECTORY_SEPARATOR . 'stream_server_reneg_limit.pem.tmp');
?>
--EXPECTF--
resource(%d) of type (stream)
