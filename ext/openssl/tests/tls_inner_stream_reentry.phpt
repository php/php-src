--TEST--
TLS over an inner stream: the inner stream cannot use or close the stream it carries
--EXTENSIONS--
openssl
--SKIPIF--
<?php
if (!function_exists('proc_open')) die('skip proc_open() not available');
?>
--FILE--
<?php
$certFile = __DIR__ . DIRECTORY_SEPARATOR . 'tls_inner_stream_reentry.pem.tmp';
include 'CertificateGenerator.inc';
(new CertificateGenerator())->saveNewCertAsFileWithKey('inner-stream', $certFile);

$serverCode = <<<'CODE'
    $ctx = stream_context_create(['ssl' => ['local_cert' => '%s']]);
    $server = stream_socket_server('tls://127.0.0.1:0', $errno, $errstr,
        STREAM_SERVER_BIND | STREAM_SERVER_LISTEN, $ctx);
    phpt_notify_server_start($server);

    $peer = stream_socket_accept($server, 5);
    $data = '';
    while (strlen($data) < 20004 && !feof($peer)) {
        $data .= fread($peer, 8192);
    }
    fwrite($peer, md5($data));
    fclose($peer);
CODE;
$serverCode = sprintf($serverCode, $certFile);

$clientCode = <<<'CODE'
    class Tun
    {
        public $context;
        public static $hook;
        private $sock;

        function stream_open($path, $mode, $options, &$opened)
        {
            $this->sock = stream_socket_client('tcp://' . substr($path, strlen('tun://')));
            return $this->sock !== false;
        }
        function stream_read($count) { return fread($this->sock, $count); }
        function stream_write($data)
        {
            if ($hook = self::$hook) {
                self::$hook = null;
                $hook();
            }
            return fwrite($this->sock, $data);
        }
        function stream_eof() { return feof($this->sock); }
        function stream_close() { fclose($this->sock); }
    }
    stream_wrapper_register('tun', 'Tun');

    $inner = fopen('tun://{{ ADDR }}', 'r+');
    $tls = stream_socket_client('tls://{{ ADDR }}', $errno, $errstr, 5, STREAM_CLIENT_CONNECT,
        stream_context_create(['ssl' => [
            'verify_peer' => false,
            'verify_peer_name' => false,
            'inner_stream' => $inner,
        ]]));

    // A write of the stream from within the write of its inner stream.
    Tun::$hook = function () use ($tls) {
        var_dump(fwrite($tls, str_repeat('B', 20000)));
        var_dump(fflush($tls));
    };
    var_dump(fwrite($tls, str_repeat('A', 20000)));

    // The inner stream closes the stream while it writes through it.
    Tun::$hook = function () use ($tls) {
        var_dump(fclose($tls));
    };
    var_dump(fwrite($tls, 'ping'));

    var_dump(fread($tls, 32) === md5(str_repeat('A', 20000) . 'ping'));
    var_dump(fclose($tls));
CODE;

include 'ServerClientTestCase.inc';
ServerClientTestCase::getInstance()->run($clientCode, $serverCode);
?>
--CLEAN--
<?php
@unlink(__DIR__ . DIRECTORY_SEPARATOR . 'tls_inner_stream_reentry.pem.tmp');
?>
--EXPECTF--
Warning: fwrite(): SSL: the stream is used from within its own operation in %s on line %d
int(0)
bool(true)
int(20000)

Warning: fclose(): cannot close the provided stream, as it must not be manually closed in %s on line %d
bool(false)
int(4)
bool(true)
bool(true)
