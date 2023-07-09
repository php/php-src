--TEST--
CURLOPT_SSL* basic client auth tests
--EXTENSIONS--
curl
--SKIPIF--
<?php
if (getenv("SKIP_SLOW_TESTS")) {
    // finished in 1.57s in a best-of-3 on an idle Intel Xeon X5670 on PHP 8.3.0-dev
    die("skip slow test");
}
if (!function_exists("proc_open")) die("skip no proc_open");
exec('openssl version', $out, $code);
if ($code > 0) die("skip couldn't locate openssl binary");
if (PHP_OS_FAMILY === 'Windows') die('skip not for Windows');
$curl_version = curl_version();
if ($curl_version['version_number'] < 0x074700) {
    die("skip: blob options not supported for curl < 7.71.0");
}
?>
--FILE--
<?php

function check_error(CurlHandle $ch) {
    if (curl_errno($ch) !== 0) {
        echo "CURL ERROR: " . curl_errno($ch) . "\n";
    }
}

function check_response($response, $clientCertSubject) {
    if (strpos($response, $clientCertSubject) === false) {
        echo "client cert subject not in response\n";
    } else {
        echo "client cert subject in response\n";
    }
}

$clientCertSubject = "Subject: C=US, ST=TX, L=Clientlocation, O=Clientcompany, CN=clientname/emailAddress=test@example.com";

// load server cert
$serverCertPath = __DIR__ . DIRECTORY_SEPARATOR . 'curl_setopt_ssl_servercert.pem';
$serverCert = file_get_contents($serverCertPath);

// load server key
$serverKeyPath = __DIR__ . DIRECTORY_SEPARATOR . 'curl_setopt_ssl_serverkey.pem';
$serverKey = file_get_contents($serverKeyPath);

// load client cert
$clientCertPath = __DIR__ . DIRECTORY_SEPARATOR . 'curl_setopt_ssl_clientcert.pem';
$clientCert = file_get_contents($clientCertPath);

// load client key
$clientKeyPath = __DIR__ . DIRECTORY_SEPARATOR . 'curl_setopt_ssl_clientkey.pem';
$clientKey = file_get_contents($clientKeyPath);

if ($serverCert === false
    || $serverKey === false
    || $clientCert === false
    || $clientKey === false
) {
    die('failed to load test certs and keys for files');
}

$port = 14430;

// set up local server
$cmd = "openssl s_server -key $serverKeyPath -cert $serverCertPath -accept $port -www -CAfile $clientCertPath -verify_return_error -Verify 1";
$process = proc_open($cmd, [["pipe", "r"], ["pipe", "w"], ["pipe", "w"]], $pipes);

if ($process === false) {
    die('failed to start server');
}
try {
    // Give the server time to start
    sleep(1);

    echo "case 1: client cert and key from string\n";
    $ch = curl_init("https://127.0.0.1:$port/");
    var_dump(curl_setopt($ch, CURLOPT_SSLCERT_BLOB, $clientCert));
    var_dump(curl_setopt($ch, CURLOPT_SSLKEY_BLOB, $clientKey));
    var_dump(curl_setopt($ch, CURLOPT_SSL_VERIFYPEER, false));
    var_dump(curl_setopt($ch, CURLOPT_SSL_VERIFYHOST, false));
    curl_setopt($ch, CURLOPT_RETURNTRANSFER, 1);

    $response = curl_exec($ch);
    check_response($response, $clientCertSubject);
    check_error($ch);
    curl_close($ch);

    echo "\n";
    echo "case 2: empty client cert and key from string\n";
    $ch = curl_init("https://127.0.0.1:$port/");
    var_dump(curl_setopt($ch, CURLOPT_SSLCERT_BLOB, ''));
    var_dump(curl_setopt($ch, CURLOPT_SSLKEY_BLOB, $clientKey));
    var_dump(curl_setopt($ch, CURLOPT_SSL_VERIFYPEER, false));
    var_dump(curl_setopt($ch, CURLOPT_SSL_VERIFYHOST, false));
    curl_setopt($ch, CURLOPT_RETURNTRANSFER, 1);

    $response = curl_exec($ch);
    check_response($response, $clientCertSubject);
    check_error($ch);
    curl_close($ch);

    echo "\n";
    echo "case 3: client cert and empty key from string\n";
    $ch = curl_init("https://127.0.0.1:$port/");
    var_dump(curl_setopt($ch, CURLOPT_SSLCERT_BLOB, $clientCert));
    var_dump(curl_setopt($ch, CURLOPT_SSLKEY_BLOB, ''));
    var_dump(curl_setopt($ch, CURLOPT_SSL_VERIFYPEER, false));
    var_dump(curl_setopt($ch, CURLOPT_SSL_VERIFYHOST, false));
    curl_setopt($ch, CURLOPT_RETURNTRANSFER, 1);

    $response = curl_exec($ch);
    check_response($response, $clientCertSubject);
    check_error($ch);
    curl_close($ch);

    echo "\n";
    echo "case 4: client cert and key from file\n";
    $ch = curl_init("https://127.0.0.1:$port/");
    var_dump(curl_setopt($ch, CURLOPT_SSLCERT, $clientCertPath));
    var_dump(curl_setopt($ch, CURLOPT_SSLKEY, $clientKeyPath));
    var_dump(curl_setopt($ch, CURLOPT_SSL_VERIFYPEER, false));
    var_dump(curl_setopt($ch, CURLOPT_SSL_VERIFYHOST, false));
    curl_setopt($ch, CURLOPT_RETURNTRANSFER, 1);

    $response = curl_exec($ch);
    check_response($response, $clientCertSubject);
    check_error($ch);
    curl_close($ch);

    echo "\n";
    echo "case 5: issuer cert from file\n";
    $ch = curl_init("https://127.0.0.1:$port/");
    var_dump(curl_setopt($ch, CURLOPT_CAINFO, $serverCertPath));
    var_dump(curl_setopt($ch, CURLOPT_ISSUERCERT, $serverCertPath));
    var_dump(curl_setopt($ch, CURLOPT_SSLCERT, $clientCertPath));
    var_dump(curl_setopt($ch, CURLOPT_SSLKEY, $clientKeyPath));
    var_dump(curl_setopt($ch, CURLOPT_SSL_VERIFYPEER, true));
    var_dump(curl_setopt($ch, CURLOPT_SSL_VERIFYHOST, false));
    curl_setopt($ch, CURLOPT_RETURNTRANSFER, 1);

    $response = curl_exec($ch);
    check_response($response, $clientCertSubject);
    check_error($ch);
    curl_close($ch);

    echo "\n";
    echo "case 6: issuer cert from string\n";
    $ch = curl_init("https://127.0.0.1:$port/");
    var_dump(curl_setopt($ch, CURLOPT_CAINFO, $serverCertPath));
    var_dump(curl_setopt($ch, CURLOPT_ISSUERCERT_BLOB, $serverCert));
    var_dump(curl_setopt($ch, CURLOPT_SSLCERT, $clientCertPath));
    var_dump(curl_setopt($ch, CURLOPT_SSLKEY, $clientKeyPath));
    var_dump(curl_setopt($ch, CURLOPT_SSL_VERIFYPEER, true));
    var_dump(curl_setopt($ch, CURLOPT_SSL_VERIFYHOST, false));
    curl_setopt($ch, CURLOPT_RETURNTRANSFER, 1);

    $response = curl_exec($ch);
    check_response($response, $clientCertSubject);
    check_error($ch);
    curl_close($ch);

    echo "\n";
    echo "case 7: empty issuer cert from string\n";
    $ch = curl_init("https://127.0.0.1:$port/");
    var_dump(curl_setopt($ch, CURLOPT_CAINFO, $serverCertPath));
    var_dump(curl_setopt($ch, CURLOPT_ISSUERCERT_BLOB, ''));
    var_dump(curl_setopt($ch, CURLOPT_SSLCERT, $clientCertPath));
    var_dump(curl_setopt($ch, CURLOPT_SSLKEY, $clientKeyPath));
    var_dump(curl_setopt($ch, CURLOPT_SSL_VERIFYPEER, true));
    var_dump(curl_setopt($ch, CURLOPT_SSL_VERIFYHOST, false));
    curl_setopt($ch, CURLOPT_RETURNTRANSFER, 1);

    $response = curl_exec($ch);
    check_response($response, $clientCertSubject);
    check_error($ch);
    curl_close($ch);

} finally {
    // clean up server process
    proc_terminate($process);
    proc_close($process);
}

?>
--EXPECT--
case 1: client cert and key from string
bool(true)
bool(true)
bool(true)
bool(true)
client cert subject in response

case 2: empty client cert and key from string
bool(true)
bool(true)
bool(true)
bool(true)
client cert subject not in response
CURL ERROR: 58

case 3: client cert and empty key from string
bool(true)
bool(true)
bool(true)
bool(true)
client cert subject not in response
CURL ERROR: 58

case 4: client cert and key from file
bool(true)
bool(true)
bool(true)
bool(true)
client cert subject in response

case 5: issuer cert from file
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
client cert subject in response

case 6: issuer cert from string
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
client cert subject in response

case 7: empty issuer cert from string
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
client cert subject not in response
CURL ERROR: 83