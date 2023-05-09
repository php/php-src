--TEST--
IPv6 Peer verification matches SAN names
--EXTENSIONS--
openssl
--SKIPIF--
<?php
if (!function_exists("proc_open")) die("skip no proc_open");
if (getenv("CI_NO_IPV6") || !defined("AF_INET6")) {
    die('skip no IPv6 support');
}
if (@stream_socket_client('udp://[::1]:8888') === false)
    die('skip no IPv6 support');
?>
--FILE--
<?php
$certFile = __DIR__ . DIRECTORY_SEPARATOR . 'san_ipv6_peer_matching.pem.tmp';
$san = 'IP:2001:db8:85a3:8d3:1319:8a2e:370:7348';

$serverCode = <<<'CODE'
    $serverUri = "ssl://[::1]:64324";
    $serverFlags = STREAM_SERVER_BIND | STREAM_SERVER_LISTEN;
    $serverCtx = stream_context_create(['ssl' => [
        'local_cert' => '%s',
    ]]);

    $server = stream_socket_server($serverUri, $errno, $errstr, $serverFlags, $serverCtx);
    phpt_notify();

    @stream_socket_accept($server, 1);
    @stream_socket_accept($server, 1);
CODE;
$serverCode = sprintf($serverCode, $certFile);

$clientCode = <<<'CODE'
    $serverUri = "ssl://[::1]:64324";
    $clientFlags = STREAM_CLIENT_CONNECT;
    $clientCtx = stream_context_create(['ssl' => [
        'verify_peer' => false,
    ]]);

    phpt_wait();

    stream_context_set_option($clientCtx, 'ssl', 'peer_name', '2001:db8:85a3:8d3:1319:8a2e:370:7348');
    var_dump(stream_socket_client($serverUri, $errno, $errstr, 1, $clientFlags, $clientCtx));

    stream_context_set_option($clientCtx, 'ssl', 'peer_name', '2001:db8:85a3:8d3:1319:8a2e:370:7349');
    var_dump(stream_socket_client($serverUri, $errno, $errstr, 1, $clientFlags, $clientCtx));
CODE;

include 'CertificateGenerator.inc';
$certificateGenerator = new CertificateGenerator();
$certificateGenerator->saveNewCertAsFileWithKey(null, $certFile, null, $san);

include 'ServerClientTestCase.inc';
ServerClientTestCase::getInstance()->run($clientCode, $serverCode);
?>
--CLEAN--
<?php
@unlink(__DIR__ . DIRECTORY_SEPARATOR . 'san_ipv6_peer_matching.pem.tmp');
?>
--EXPECTF--
resource(%d) of type (stream)

Warning: stream_socket_client(): Unable to locate peer certificate CN in %s on line %d

Warning: stream_socket_client(): Failed to enable crypto in %s on line %d

Warning: stream_socket_client(): Unable to connect to ssl://[::1]:64324 (Unknown error) in %s on line %d
bool(false)
