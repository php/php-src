--TEST--
GHSA-hgf5-96fm-v528: Header parser of http stream wrapper does not handle folded headers (correct start pos)
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

    phpt_notify(message:"server-accepted");

    $result = fread($conn, 1024);
    $encoded_result = base64_encode($result);

    fwrite($conn, "HTTP/1.0 200 Ok\r\nContent-Type: text/html; charset=utf-8\r\n\r\n$encoded_result\r\n");

CODE;

$clientCode = <<<'CODE'
    $opts = [
        "http" => [
            "method" => "GET",
            "header" => "Authorization: Bearer x\r\n"
        ]
    ];
    $ctx = stream_context_create($opts);
    var_dump(explode("\r\n", base64_decode(file_get_contents("http://user:pwd@{{ ADDR }}", false, $ctx))));
    var_dump(http_get_last_response_headers());
CODE;

include sprintf("%s/../../../openssl/tests/ServerClientTestCase.inc", __DIR__);
ServerClientTestCase::getInstance()->run($clientCode, $serverCode);
?>
--EXPECTF--
array(6) {
  [0]=>
  string(14) "GET / HTTP/1.1"
  [1]=>
  string(%d) "Host: 127.0.0.1:%d"
  [2]=>
  string(17) "Connection: close"
  [3]=>
  string(23) "Authorization: Bearer x"
  [4]=>
  string(0) ""
  [5]=>
  string(0) ""
}
array(2) {
  [0]=>
  string(15) "HTTP/1.0 200 Ok"
  [1]=>
  string(38) "Content-Type: text/html; charset=utf-8"
}
