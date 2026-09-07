--TEST--
Empty Location header must not over-read when building the redirect target
--FILE--
<?php
$serverCode = <<<'CODE'
$server = stream_socket_server("tcp://127.0.0.1:0", $errno, $errstr);
phpt_notify_server_start($server);

for ($n = 0; $n < 4; $n++) {
    $conn = stream_socket_accept($server, 10);
    if (!$conn) {
        break;
    }
    $req = fgets($conn);
    while (trim(fgets($conn)) !== '') {}
    $uri = explode(' ', $req)[1];
    if ($n < 3) {
        fwrite($conn, "HTTP/1.1 302 Found\r\nLocation:\r\nContent-Length: 0\r\n\r\n");
    } else {
        $body = "uri=$uri";
        fwrite($conn, "HTTP/1.1 200 OK\r\nContent-Length: " . strlen($body) . "\r\n\r\n$body");
    }
    fclose($conn);
}
CODE;

$clientCode = <<<'CODE'
$ctx = stream_context_create(['http' => ['follow_location' => 1]]);
echo @file_get_contents("http://{{ ADDR }}/a/b", false, $ctx), "\n";
CODE;

include sprintf("%s/../../../openssl/tests/ServerClientTestCase.inc", __DIR__);
ServerClientTestCase::getInstance()->run($clientCode, $serverCode);
?>
--EXPECT--
uri=/
