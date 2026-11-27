--TEST--
GH-25727 (CURLOPT_CAINFO and CURLOPT_CAPATH do not accept null)
--EXTENSIONS--
curl
--SKIPIF--
<?php
if (!function_exists("proc_open")) die("skip no proc_open");
exec('openssl version', $out, $code);
if ($code > 0) die("skip couldn't locate openssl binary");
if (PHP_OS_FAMILY === 'Windows') die('skip not for Windows');
if (PHP_OS_FAMILY === 'Darwin') die('skip Fails intermittently on macOS');
if (PHP_OS === 'FreeBSD') die('skip proc_open seems to be stuck on FreeBSD');
if (!str_contains(curl_version()['ssl_version'], 'OpenSSL')) {
    die('skip CURLOPT_CAPATH with a hashed directory needs libcurl built with OpenSSL');
}
?>
--FILE--
<?php

function request(int $port, array ...$optionSets): void {
    $ch = curl_init("https://127.0.0.1:$port/");
    curl_setopt($ch, CURLOPT_RETURNTRANSFER, true);
    curl_setopt($ch, CURLOPT_SSL_VERIFYPEER, true);
    /* The test certificate has no subject alternative name. */
    curl_setopt($ch, CURLOPT_SSL_VERIFYHOST, 0);
    foreach ($optionSets as $options) {
        var_dump(curl_setopt_array($ch, $options));
    }
    var_dump(curl_exec($ch) !== false);
    var_dump(curl_errno($ch));
}

$serverCertPath = __DIR__ . DIRECTORY_SEPARATOR . 'curl_setopt_ssl_servercert.pem';
$serverKeyPath = __DIR__ . DIRECTORY_SEPARATOR . 'curl_setopt_ssl_serverkey.pem';

/* A CA directory in the layout OpenSSL looks up: <subject hash>.0 */
$caPath = __DIR__ . DIRECTORY_SEPARATOR . 'gh25727_capath';
$hash = exec("openssl x509 -noout -subject_hash -in $serverCertPath");
@mkdir($caPath);
copy($serverCertPath, $caPath . DIRECTORY_SEPARATOR . $hash . '.0');

$port = 14431;

$cmd = "openssl s_server -key $serverKeyPath -cert $serverCertPath -accept $port -www";
$process = proc_open($cmd, [["pipe", "r"], ["pipe", "w"], ["pipe", "w"]], $pipes);

if ($process === false) {
    die('failed to start server');
}
try {
    // Wait until OpenSSL is actually listening before making the first request.
    $ready = false;
    for ($i = 0; $i < 100; $i++) {
        $connection = @stream_socket_client("tcp://127.0.0.1:$port", $errno, $errstr, 0.05);
        if ($connection !== false) {
            fclose($connection);
            $ready = true;
            break;
        }
        if (!proc_get_status($process)['running']) {
            break;
        }
        usleep(50000);
    }
    if (!$ready) {
        stream_set_blocking($pipes[2], false);
        throw new RuntimeException('OpenSSL server did not start: ' . trim(stream_get_contents($pipes[2])));
    }

    echo "CAINFO disabled, certificate found in CAPATH\n";
    request($port, [CURLOPT_CAINFO => null, CURLOPT_CAPATH => $caPath]);

    echo "CAPATH disabled, certificate found in CAINFO\n";
    request($port, [CURLOPT_CAINFO => $serverCertPath, CURLOPT_CAPATH => null]);

    echo "CAINFO set and then disabled, CAPATH disabled\n";
    request($port, [CURLOPT_CAINFO => $serverCertPath], [CURLOPT_CAINFO => null, CURLOPT_CAPATH => null]);
} finally {
    proc_terminate($process);
    proc_close($process);
}

?>
--CLEAN--
<?php
$caPath = __DIR__ . DIRECTORY_SEPARATOR . 'gh25727_capath';
array_map('unlink', glob($caPath . DIRECTORY_SEPARATOR . '*.0'));
@rmdir($caPath);
?>
--EXPECT--
CAINFO disabled, certificate found in CAPATH
bool(true)
bool(true)
int(0)
CAPATH disabled, certificate found in CAINFO
bool(true)
bool(true)
int(0)
CAINFO set and then disabled, CAPATH disabled
bool(true)
bool(true)
bool(false)
int(60)
