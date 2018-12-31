--TEST--
Bug #54992: Stream not closed and error not returned when SSL CN_match fails
--SKIPIF--
<?php 
if (!extension_loaded("openssl")) die("skip openssl not loaded");
if (!function_exists("proc_open")) die("skip no proc_open");
--FILE--
<?php
/*
 How to generate bug54992.pem and bug54992-ca.pem and all dependants:

 All the commands below assume you're in the root of php sources

 Generate new key for CA:
 $ openssl genrsa -out ./ext/openssl/tests/bug54992-ca.key 4096

 Create new CA:
 $ openssl req -new -x509 -key ./ext/openssl/tests/bug54992-ca.key \
      -out ext/openssl/tests/bug54992-ca.pem \
      -subj '/C=PT/ST=Lisboa/L=Lisboa/O=PHP Foundation/CN=Root CA for PHP Tests/emailAddress=internals@lists.php.net' \
      -days 400

 Extract private key from the bundle:
 $ openssl rsa -in ext/openssl/tests/bug54992.pem > ext/openssl/tests/bug54992.key

 Extract CSR from existing certificate:
 $ openssl x509 -x509toreq -in ext/openssl/tests/bug54992.pem -out ext/openssl/tests/bug54992.csr -signkey ext/openssl/tests/bug54992.key

 Sign the CSR:
 $ openssl x509 -CA ext/openssl/tests/bug54992-ca.pem \
        -CAcreateserial \
        -CAkey ./ext/openssl/tests/bug54992-ca.key \
        -req \
        -in ext/openssl/tests/bug54992.csr \
        -sha256 \
        -days 400 \
        -out ./ext/openssl/tests/bug54992.pem

 Bundle certificate's private key with the certificate:
 $ cat ext/openssl/tests/bug54992.key >> ext/openssl/tests/bug54992.pem\


 Dependants:

 1. ext/openssl/tests/bug65538_003.phpt
    Run the following to generate required phar:
    php -d phar.readonly=Off -r '$phar = new Phar("ext/openssl/tests/bug65538.phar"); $phar->addFile("ext/openssl/tests/bug54992.pem", "bug54992.pem"); $phar->addFile("ext/openssl/tests/bug54992-ca.pem", "bug54992-ca.pem");'

 2. Update ext/openssl/tests/openssl_peer_fingerprint_basic.phpt (see instructions in there)
 */
$serverCode = <<<'CODE'
    $serverUri = "ssl://127.0.0.1:64321";
    $serverFlags = STREAM_SERVER_BIND | STREAM_SERVER_LISTEN;
    $serverCtx = stream_context_create(['ssl' => [
        'local_cert' => __DIR__ . '/bug54992.pem',
    ]]);

    $server = stream_socket_server($serverUri, $errno, $errstr, $serverFlags, $serverCtx);
    phpt_notify();

    @stream_socket_accept($server, 1);
CODE;

$clientCode = <<<'CODE'
    $serverUri = "ssl://127.0.0.1:64321";
    $clientFlags = STREAM_CLIENT_CONNECT;
    $clientCtx = stream_context_create(['ssl' => [
        'verify_peer' => true,
        'cafile' => __DIR__ . '/bug54992-ca.pem',
        'peer_name' => 'buga_buga',
    ]]);

    phpt_wait();
    $client = stream_socket_client($serverUri, $errno, $errstr, 2, $clientFlags, $clientCtx);

    var_dump($client);
CODE;

include 'ServerClientTestCase.inc';
ServerClientTestCase::getInstance()->run($clientCode, $serverCode);
--EXPECTF--
Warning: stream_socket_client(): Peer certificate CN=`bug54992.local' did not match expected CN=`buga_buga' in %s on line %d

Warning: stream_socket_client(): Failed to enable crypto in %s on line %d

Warning: stream_socket_client(): unable to connect to ssl://127.0.0.1:64321 (Unknown error) in %s on line %d
bool(false)


