--TEST--
OOM during request parsing does not leak into the next request
--FILE--
<?php
include "php_cli_server.inc";
php_cli_server_start(<<<'PHP'
spl_autoload_register(function ($class) {
    eval("class $class {}");
});
ob_start();
echo get_class(new Foo), "\n";
trigger_error("Bar", E_USER_WARNING);
ob_end_flush();
PHP, 'index.php', ['-d', 'memory_limit=8M', '-d', 'post_max_size=64M', '-d', 'html_errors=0']);

function send(string $request): string {
    $fp = php_cli_server_connect();
    fwrite($fp, $request);
    $response = stream_get_contents($fp);
    fclose($fp);

    return $response;
}

$body = 'a=' . str_repeat('a', 6 * 1024 * 1024);
$response = send("POST / HTTP/1.1\r\nHost: localhost\r\nContent-Type: application/x-www-form-urlencoded\r\nContent-Length: " . strlen($body) . "\r\n\r\n" . $body);
var_dump(str_contains($response, 'Allowed memory size of 8388608 bytes exhausted'));

$response = send("GET / HTTP/1.1\r\nHost: localhost\r\n\r\n");
echo substr($response, strpos($response, "\r\n\r\n") + 4);
?>
--EXPECTF--
bool(true)
Foo

Warning: Bar in %s on line %d
