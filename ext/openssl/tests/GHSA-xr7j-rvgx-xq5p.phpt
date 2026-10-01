--TEST--
GHSA-xr7j-rvgx-xq5p: OOB read in php_openssl_matches_wildcard_name() with overlapping wildcard prefix and suffix
--EXTENSIONS--
openssl
--SKIPIF--
<?php
if (!function_exists("proc_open")) die("skip no proc_open");
?>
--FILE--
<?php
$certFile = __DIR__ . DIRECTORY_SEPARATOR . 'GHSA-xr7j-rvgx-xq5p.pem.tmp';
$san = 'DNS:a*aaaa, DNS:w*www.example.org';

$serverCode = <<<'CODE'
    $serverUri = "ssl://127.0.0.1:0";
    $serverFlags = STREAM_SERVER_BIND | STREAM_SERVER_LISTEN;
    $serverCtx = stream_context_create(['ssl' => [
        'local_cert' => '%s',
    ]]);

    $server = stream_socket_server($serverUri, $errno, $errstr, $serverFlags, $serverCtx);
    phpt_notify_server_start($server);

    @stream_socket_accept($server, 1);
    @stream_socket_accept($server, 1);
CODE;
$serverCode = sprintf($serverCode, $certFile);

$clientCode = <<<'CODE'
    $serverUri = "ssl://{{ ADDR }}";
    $clientFlags = STREAM_CLIENT_CONNECT;
    $clientCtx = stream_context_create(['ssl' => [
        'verify_peer' => false,
    ]]);

    stream_context_set_option($clientCtx, 'ssl', 'peer_name', 'aaaa');
    var_dump(stream_socket_client($serverUri, $errno, $errstr, 1, $clientFlags, $clientCtx));

    stream_context_set_option($clientCtx, 'ssl', 'peer_name', 'www.example.org');
    var_dump(stream_socket_client($serverUri, $errno, $errstr, 1, $clientFlags, $clientCtx));
CODE;

include 'CertificateGenerator.inc';
$certificateGenerator = new CertificateGenerator();
$certificateGenerator->saveNewCertAsFileWithKey('a*aaaa', $certFile, null, $san);

include 'ServerClientTestCase.inc';
ServerClientTestCase::getInstance()->run($clientCode, $serverCode);
?>
--CLEAN--
<?php
@unlink(__DIR__ . DIRECTORY_SEPARATOR . 'GHSA-xr7j-rvgx-xq5p.pem.tmp');
?>
--EXPECTF--
Warning: stream_socket_client(): Peer certificate subjectAltName did not match expected name `aaaa' in %s on line %d

Warning: stream_socket_client(): Failed to enable crypto in %s on line %d

Warning: stream_socket_client(): Unable to connect to ssl://127.0.0.1:%d (Unknown error) in %s on line %d
bool(false)

Warning: stream_socket_client(): Peer certificate subjectAltName did not match expected name `www.example.org' in %s on line %d

Warning: stream_socket_client(): Failed to enable crypto in %s on line %d

Warning: stream_socket_client(): Unable to connect to ssl://127.0.0.1:%d (Unknown error) in %s on line %d
bool(false)
