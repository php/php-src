--TEST--
GHSA-hgf5-96fm-v528: Stream HTTP wrapper header check might omit basic auth header (incorrect inside pos)
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
            "header" => "Cookie: foo=bar\nauthorization:x\r\n"
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
array(7) {
  [0]=>
  string(14) "GET / HTTP/1.1"
  [1]=>
  string(33) "Authorization: Basic dXNlcjpwd2Q="
  [2]=>
  string(21) "Host: 127.0.0.1:%d"
  [3]=>
  string(17) "Connection: close"
  [4]=>
  string(31) "Cookie: foo=bar
authorization:x"
  [5]=>
  string(0) ""
  [6]=>
  string(0) ""
}
array(2) {
  [0]=>
  string(15) "HTTP/1.0 200 Ok"
  [1]=>
  string(38) "Content-Type: text/html; charset=utf-8"
}
