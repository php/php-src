--TEST--
stream_get_channel_binding(): TLS 1.3, tls-unique is not applicable
--EXTENSIONS--
openssl
--SKIPIF--
<?php
if (!function_exists("proc_open")) die("skip no proc_open");
?>
--FILE--
<?php
$certFile = __DIR__ . DIRECTORY_SEPARATOR . 'cb_tls13.pem.tmp';

$serverCode = <<<'CODE'
    $ctx = stream_context_create(['ssl' => [
        'local_cert' => '%s',
        'verify_peer' => false,
        'verify_peer_name' => false,
        'capture_peer_cert' => true,
        'security_level' => 0,
    ]]);
    $flags = STREAM_SERVER_BIND|STREAM_SERVER_LISTEN;
    $server = stream_socket_server("tlsv1.3://127.0.0.1:0", $errno, $errstr, $flags, $ctx);
    phpt_notify_server_start($server);

    $conn = stream_socket_accept($server, 30);
    if ($conn === false) {
        echo "SERVER_EXCEPTION accept failed\n";
        exit(1);
    }
    foreach (["tls-unique", "tls-server-endpoint", "tls-exporter"] as $t) {
        $v = stream_get_channel_binding($conn, $t);
        fwrite($conn, $t . "=" . (is_string($v) ? bin2hex($v) : "") . "\n");
    }
    phpt_wait();
    fclose($conn);
CODE;
$serverCode = sprintf($serverCode, $certFile);

$clientCode = <<<'CODE'
    $ctx = stream_context_create(['ssl' => [
        'verify_peer' => false,
        'verify_peer_name' => false,
        'capture_peer_cert' => true,
        'security_level' => 0,
    ]]);
    $client = stream_socket_client("tlsv1.3://{{ ADDR }}", $errno, $errstr, 30, STREAM_CLIENT_CONNECT, $ctx);
    if ($client === false) {
        echo "client connect failed\n";
        exit(1);
    }

    $my = [];
    foreach (["tls-unique", "tls-server-endpoint", "tls-exporter"] as $t) {
        $my[$t] = stream_get_channel_binding($client, $t);
    }

    $peer = [];
    for ($i = 0; $i < 3; $i++) {
        $line = fgets($client);
        if ($line === false) break;
        $line = rtrim($line);
        $eq = strpos($line, "=");
        $t = substr($line, 0, $eq);
        $enc = substr($line, $eq + 1);
        $peer[$t] = ($enc === "") ? null : hex2bin($enc);
    }

    foreach (["tls-unique", "tls-server-endpoint", "tls-exporter"] as $t) {
        $c = $my[$t];
        $s = $peer[$t];
        $lc = is_string($c) ? strlen($c) : "null";
        $ls = is_string($s) ? strlen($s) : "null";
        printf("%s equal=%s len_client=%s len_server=%s\n",
            $t, var_export($c === $s, true), $lc, $ls);
    }

    echo "tls_unique_is_null=" . var_export($my["tls-unique"] === null, true) . "\n";

    $opts = stream_context_get_options($client);
    $cert = $opts['ssl']['peer_certificate'] ?? null;
    $fpr = $cert ? openssl_x509_fingerprint($cert, "sha256", true) : null;
    echo "tse_matches_sha256_fingerprint="
        . var_export($fpr !== null && $my["tls-server-endpoint"] === $fpr, true) . "\n";

    phpt_notify('server');
    fclose($client);
CODE;

include 'CertificateGenerator.inc';
$generator = new CertificateGenerator();
$generator->saveNewCertAsFileWithKey('cb-tls13', $certFile);

include 'ServerClientTestCase.inc';
ServerClientTestCase::getInstance()->run($clientCode, ['server' => $serverCode]);
?>
--CLEAN--
<?php
@unlink(__DIR__ . DIRECTORY_SEPARATOR . 'cb_tls13.pem.tmp');
?>
--EXPECT--
tls-unique equal=true len_client=null len_server=null
tls-server-endpoint equal=true len_client=32 len_server=32
tls-exporter equal=true len_client=32 len_server=32
tls_unique_is_null=true
tse_matches_sha256_fingerprint=true
