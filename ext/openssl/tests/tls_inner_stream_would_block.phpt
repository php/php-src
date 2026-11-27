--TEST--
TLS over an inner stream that would block: the connect fails instead of sending plaintext
--EXTENSIONS--
openssl
--FILE--
<?php
$certFile = __DIR__ . DIRECTORY_SEPARATOR . 'tls_inner_stream_would_block.pem.tmp';
include 'CertificateGenerator.inc';
(new CertificateGenerator())->saveNewCertAsFileWithKey('inner-stream', $certFile);

class Pending
{
    public $context;
    public static $sent = '';
    private $sock;

    function stream_open($path, $mode, $options, &$opened)
    {
        $this->sock = stream_socket_client('tcp://' . substr($path, strlen('pending://')));
        return $this->sock !== false;
    }
    // An asynchronous tunnel with nothing received yet.
    function stream_read($count) { return ''; }
    function stream_write($data)
    {
        self::$sent .= $data;
        return fwrite($this->sock, $data);
    }
    function stream_eof() { return false; }
    function stream_close() { fclose($this->sock); }
}
stream_wrapper_register('pending', 'Pending');

$ssl = ['verify_peer' => false, 'verify_peer_name' => false];

// A non-blocking inner stream: the server has not answered the ClientHello yet.
$server = stream_socket_server('tcp://127.0.0.1:0', $errno, $errstr);
$addr = stream_socket_get_name($server, false);
$inner = stream_socket_client("tcp://$addr");
stream_set_blocking($inner, false);
$tls = stream_socket_client("tls://$addr", $errno, $errstr, 2, STREAM_CLIENT_CONNECT,
    stream_context_create(['ssl' => $ssl + ['inner_stream' => $inner]]));
var_dump($tls);
if ($tls) {
    fwrite($tls, "secret");
}
$peer = stream_socket_accept($server);
stream_set_blocking($peer, false);
usleep(100000);
var_dump(str_contains((string) fread($peer, 65536), 'secret'));

// A user wrapper that has nothing to read yet.
$server = stream_socket_server('tcp://127.0.0.1:0', $errno, $errstr);
$addr = stream_socket_get_name($server, false);
$inner = fopen("pending://$addr", 'r+');
$tls = stream_socket_client("tls://$addr", $errno, $errstr, 2, STREAM_CLIENT_CONNECT,
    stream_context_create(['ssl' => $ssl + ['inner_stream' => $inner]]));
var_dump($tls);
if ($tls) {
    fwrite($tls, "secret");
}
var_dump(str_contains(Pending::$sent, 'secret'));

// An asynchronous connect still hands the handshake over to the caller.
$server = stream_socket_server('tcp://127.0.0.1:0', $errno, $errstr);
$addr = stream_socket_get_name($server, false);
$inner = stream_socket_client("tcp://$addr");
stream_set_blocking($inner, false);
$tls = stream_socket_client("tls://$addr", $errno, $errstr, 2,
    STREAM_CLIENT_CONNECT | STREAM_CLIENT_ASYNC_CONNECT,
    stream_context_create(['ssl' => $ssl + ['inner_stream' => $inner]]));
stream_set_blocking($tls, false);
$peer = stream_socket_accept($server);
stream_set_blocking($peer, false);
stream_context_set_option($peer, 'ssl', 'local_cert', $certFile);
$client = $srv = 0;
for ($i = 0; $i < 200 && ($client !== true || $srv !== true); $i++) {
    if ($client !== true) {
        $client = stream_socket_enable_crypto($tls, true, STREAM_CRYPTO_METHOD_TLS_CLIENT);
    }
    if ($srv !== true) {
        $srv = stream_socket_enable_crypto($peer, true, STREAM_CRYPTO_METHOD_TLS_SERVER);
    }
    usleep(1000);
}
var_dump($client, $srv);
fwrite($tls, "ping");
usleep(100000);
var_dump(fread($peer, 4));
?>
--CLEAN--
<?php
@unlink(__DIR__ . DIRECTORY_SEPARATOR . 'tls_inner_stream_would_block.pem.tmp');
?>
--EXPECTF--
Warning: stream_socket_client(): Failed to enable crypto in %s on line %d

Warning: stream_socket_client(): Unable to connect to tls://%s in %s on line %d
bool(false)
bool(false)

Warning: stream_socket_client(): Failed to enable crypto in %s on line %d

Warning: stream_socket_client(): Unable to connect to tls://%s in %s on line %d
bool(false)
bool(false)
bool(true)
bool(true)
string(4) "ping"
