--TEST--
OOM raised after a shutdown function does not leak into the next request
--FILE--
<?php
include "php_cli_server.inc";
php_cli_server_start(<<<'PHP'
if (isset($_GET['oom'])) {
    register_shutdown_function(function () {
        $a = str_repeat('a', 3 * 1024 * 1024);
        $GLOBALS['b'] = $a . $a;
    });
    return;
}
spl_autoload_register(function ($class) {
    eval("class $class {}");
});
echo get_class(new Foo), "\n";
PHP, 'index.php', ['-d', 'memory_limit=8M', '-d', 'html_errors=0']);

function send(string $request): string {
    $fp = php_cli_server_connect();
    fwrite($fp, $request);
    $response = stream_get_contents($fp);
    fclose($fp);

    return $response;
}

$response = send("GET /?oom=1 HTTP/1.1\r\nHost: localhost\r\n\r\n");
var_dump(str_contains($response, 'Allowed memory size of 8388608 bytes exhausted'));

$response = send("GET / HTTP/1.1\r\nHost: localhost\r\n\r\n");
echo substr($response, strpos($response, "\r\n\r\n") + 4);
?>
--EXPECT--
bool(true)
Foo
