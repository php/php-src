--TEST--
Single-char relative Location header keeps resolving against the host root (pre-GH-23467 behavior)
--DESCRIPTION--
Not RFC 3986 compliant ("x" against "/a/b" gives "/a/x"), but matches
PHP's long-standing behavior of resolving against the host root. See GH-23521.
--FILE--
<?php
$serverCode = <<<'CODE'
$server = stream_socket_server("tcp://127.0.0.1:0", $errno, $errstr);
phpt_notify_server_start($server);

for ($n = 0; $n < 2; $n++) {
    $conn = stream_socket_accept($server, 10);
    if (!$conn) {
        break;
    }
    $req = fgets($conn);
    while (trim(fgets($conn)) !== '') {}
    $uri = explode(' ', $req)[1];
    if ($n < 1) {
        fwrite($conn, "HTTP/1.1 302 Found\r\nLocation: x\r\nContent-Length: 0\r\n\r\n");
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
uri=/x
