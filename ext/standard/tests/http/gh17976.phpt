--TEST--
GH-17976 (CRLF injection via from and user_agent INI settings in HTTP wrapper)
--INI--
allow_url_fopen=1
--FILE--
<?php
$serverCode = <<<'CODE'
    $ctxt = stream_context_create([
        "socket" => ["tcp_nodelay" => true]
    ]);

    $server = stream_socket_server(
        "tcp://127.0.0.1:0", $errno, $errstr, STREAM_SERVER_BIND | STREAM_SERVER_LISTEN, $ctxt);
    phpt_notify_server_start($server);

    for ($i = 0; $i < 4; $i++) {
        $conn = stream_socket_accept($server);
        $result = fread($conn, 4096);
        fwrite($conn, "HTTP/1.0 200 OK\r\nContent-Type: text/plain\r\n\r\n" . base64_encode($result));
        fclose($conn);
    }
CODE;

$clientCode = <<<'CODE'
    // Test 1: from INI with CRLF
    ini_set("from", "test\r\nInjected-From: evil");
    ini_set("user_agent", "clean_ua");
    $raw = base64_decode(file_get_contents("http://{{ ADDR }}/"));
    echo (str_contains($raw, "Injected-From:") ? "FAIL" : "OK") . ": from INI\n";

    // Test 2: user_agent INI with CRLF
    ini_restore("from");
    ini_set("user_agent", "test\r\nInjected-UA: evil");
    $raw = base64_decode(file_get_contents("http://{{ ADDR }}/"));
    echo (str_contains($raw, "Injected-UA:") ? "FAIL" : "OK") . ": user_agent INI\n";

    // Test 3: user_agent context option with CRLF
    ini_restore("user_agent");
    $ctx = stream_context_create(["http" => [
        "user_agent" => "test\nInjected-Ctx: evil"
    ]]);
    $raw = base64_decode(file_get_contents("http://{{ ADDR }}/", false, $ctx));
    echo (str_contains($raw, "Injected-Ctx:") ? "FAIL" : "OK") . ": user_agent context\n";

    // Test 4: user_agent INI with a NUL byte before the CRLF
    ini_set("user_agent", "ua\0valid\r\nInjected-Nul: evil");
    $raw = base64_decode(file_get_contents("http://{{ ADDR }}/"));
    echo (str_contains($raw, "Injected-Nul:") ? "FAIL" : "OK") . ": user_agent NUL+CRLF\n";
CODE;

include sprintf("%s/../../../openssl/tests/ServerClientTestCase.inc", __DIR__);
ServerClientTestCase::getInstance()->run($clientCode, $serverCode);
?>
--EXPECTF--
Warning: file_get_contents(): Header From value contains newline characters and has been truncated in %s on line %d
OK: from INI

Warning: file_get_contents(): Header User-Agent value contains newline characters and has been truncated in %s on line %d
OK: user_agent INI

Warning: file_get_contents(): Header User-Agent value contains newline characters and has been truncated in %s on line %d
OK: user_agent context

Warning: file_get_contents(): Header User-Agent value contains newline characters and has been truncated in %s on line %d
OK: user_agent NUL+CRLF
