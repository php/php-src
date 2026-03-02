--TEST--
stream context tcp_nodelay server
--EXTENSIONS--
sockets
--SKIPIF--
<?php
if (!function_exists("proc_open")) die("skip no proc_open");
?>
--FILE--
<?php
$serverCode = <<<'CODE'
   $ctxt = stream_context_create([
        "socket" => [
            "tcp_nodelay" => true
        ]
    ]);

    $server = stream_socket_server(
        "tcp://127.0.0.1:0", $errno, $errstr, STREAM_SERVER_BIND | STREAM_SERVER_LISTEN, $ctxt);
    phpt_notify_server_start($server);

    $conn = stream_socket_accept($server);

    $si = socket_get_option(socket_import_stream($server), SOL_TCP, TCP_NODELAY) > 0 ? "nodelay": "delay";
    $ci = socket_get_option(socket_import_stream($conn), SOL_TCP, TCP_NODELAY) > 0 ? "nodelay": "delay";

    phpt_notify(message:"server-$si:conn-$ci");
CODE;

$clientCode = <<<'CODE'
    $test = stream_socket_client("tcp://{{ ADDR }}", $errno, $errstr, 10);
    echo phpt_wait();
CODE;

include sprintf("%s/../../../openssl/tests/ServerClientTestCase.inc", __DIR__);
ServerClientTestCase::getInstance()->run($clientCode, $serverCode);
?>
--EXPECTF--
server-delay:conn-nodelay

