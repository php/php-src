--TEST--
TLS over an inner stream of a user wrapper
--EXTENSIONS--
openssl
--SKIPIF--
<?php
if (!function_exists('proc_open')) die('skip proc_open() not available');
?>
--FILE--
<?php
$certFile = __DIR__ . DIRECTORY_SEPARATOR . 'tls_inner_stream_user_wrapper.pem.tmp';
include 'CertificateGenerator.inc';
(new CertificateGenerator())->saveNewCertAsFileWithKey('inner-stream', $certFile);

$serverCode = <<<'CODE'
    $ctx = stream_context_create(['ssl' => ['local_cert' => '%s']]);
    $server = stream_socket_server('tls://127.0.0.1:0', $errno, $errstr,
        STREAM_SERVER_BIND | STREAM_SERVER_LISTEN, $ctx);
    phpt_notify_server_start($server);

    $peer = stream_socket_accept($server, 5);
    fwrite($peer, strtoupper(fread($peer, 8192)));
    fclose($peer);
    fclose($server);
CODE;
$serverCode = sprintf($serverCode, $certFile);

$clientCode = <<<'CODE'
    class Tunnel {
        public $context;
        private $sock;
        public static int $reads = 0;
        public static int $writes = 0;

        public function stream_open($path, $mode, $options, &$opened) {
            $this->sock = stream_socket_client('tcp://' . substr($path, strlen('tunnel://')),
                $errno, $errstr, 5);
            return $this->sock !== false;
        }
        public function stream_read($count) {
            self::$reads++;
            return fread($this->sock, $count);
        }
        public function stream_write($data) {
            self::$writes++;
            return fwrite($this->sock, $data);
        }
        public function stream_eof() {
            return feof($this->sock);
        }
        public function stream_close() {
            fclose($this->sock);
        }
    }
    stream_wrapper_register('tunnel', 'Tunnel');

    $inner = fopen('tunnel://{{ ADDR }}', 'r+');
    $ctx = stream_context_create(['ssl' => [
        'verify_peer' => false,
        'verify_peer_name' => false,
        'inner_stream' => $inner,
    ]]);
    $tls = stream_socket_client('tls://{{ ADDR }}', $errno, $errstr, 5, STREAM_CLIENT_CONNECT, $ctx);
    var_dump($tls !== false);
    fwrite($tls, "ping");
    var_dump(fread($tls, 4));
    var_dump(Tunnel::$reads > 0 && Tunnel::$writes > 0);
    fclose($tls);
    fclose($inner);
CODE;

include 'ServerClientTestCase.inc';
ServerClientTestCase::getInstance()->run($clientCode, $serverCode);
?>
--CLEAN--
<?php
@unlink(__DIR__ . DIRECTORY_SEPARATOR . 'tls_inner_stream_user_wrapper.pem.tmp');
?>
--EXPECT--
bool(true)
string(4) "PING"
bool(true)
