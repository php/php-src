--TEST--
GH-20802: undefined behavior with invalid SNI_server_certs option values
--EXTENSIONS--
openssl
--SKIPIF--
<?php
if (!function_exists("proc_open")) die("skip no proc_open");
?>
--FILE--
<?php
$certFile = __DIR__ . DIRECTORY_SEPARATOR . 'gh20802.pem.tmp';
$serverCode = <<<'CODE'
    class A {};
    $localhost = new A();
    ini_set('log_errors', 'On');
    $flags = STREAM_SERVER_BIND|STREAM_SERVER_LISTEN;
    $ctx = stream_context_create([
	    'ssl' => [
		    'local_cert'        => '%s',
		    'allow_self_signed' => true,
		    'verify_peer_name'  => false,
		    'verify_peer'  => false,
		    'SNI_enabled' => true,
		    'SNI_server_certs' => [
			    'localhost' => &$localhost,
		    ]
	    ]
    ]);
    $server = stream_socket_server('tls://127.0.0.1:12443', $errno, $errstr, $flags, $ctx);
    phpt_notify_server_start($server);
    stream_socket_accept($server, 3);
CODE;
$serverCode = sprintf($serverCode, $certFile);

$clientCode = <<<'CODE'
    $flags = STREAM_CLIENT_CONNECT;

$ctx = stream_context_create([
	'ssl' => [
	    'SNI_enabled' => true,
	    'verify_peer_name'  => false,
	    'verify_peer'  => false
	]
    ]);
    @stream_socket_client("tls://127.0.0.1:12443", $errno, $errstr, 1, $flags, $ctx);
CODE;

include 'CertificateGenerator.inc';
$certificateGenerator = new CertificateGenerator();
$certificateGenerator->saveNewCertAsFileWithKey('gh20802-snioptions', $certFile);

include 'ServerClientTestCase.inc';
ServerClientTestCase::getInstance()->run($clientCode, $serverCode);
?>
--CLEAN--
<?php
@unlink(__DIR__ . DIRECTORY_SEPARATOR . 'gh20802.pem.tmp');
?>
--EXPECTF--
%a
PHP Warning:  stream_socket_accept(): Unable to set local cert chain file `%s'; Check that your cafile/capath settings include details of your certificate and its issuer in %s(%d) : eval()'d code on line %d
PHP Warning:  stream_socket_accept(): Failed to enable crypto in %s(%d) : eval()'d code on line %d
PHP Warning:  stream_socket_accept(): Accept failed: Cannot enable crypto in %s(%d) : eval()'d code on line %d
