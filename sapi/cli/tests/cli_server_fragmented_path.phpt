--TEST--
CLI server normalizes complete paths spanning multiple reads
--SKIPIF--
<?php
include 'skipif.inc';
?>
--FILE--
<?php
include 'php_cli_server.inc';
php_cli_server_start(<<<'PHP'
echo $_SERVER['SCRIPT_NAME'], "\n", $_SERVER['QUERY_STRING'], "\n", $_SERVER['REQUEST_URI'];
PHP, null);

foreach ([
    'unsplit' => ['/a/../%69ndex.php?check=ok'],
    'percent escape' => ['/a/../%', '69ndex.php?check=ok'],
    'percent digit' => ['/a/../%6', '9ndex.php?check=ok'],
    'parent segment' => ['/a/.', './index.php?check=ok'],
    'encoded parent segment' => ['/a/%2e%', '2e/index.php?check=ok'],
    'encoded slash' => ['/a/..%2', 'findex.php?check=ok'],
    'query boundary' => ['/index.php', '?check=', 'ok'],
    'large path' => ['/' . str_repeat('./', 9000) . 'index.php?check=ok'],
] as $name => $parts) {
    $socket = php_cli_server_connect();
    fwrite($socket, 'GET ');
    foreach ($parts as $i => $part) {
        fwrite($socket, $part);
        if ($i < count($parts) - 1) {
            // Let the server process this fragment before sending the next one.
            $control = php_cli_server_connect();
            fwrite($control, "GET /index.php?control HTTP/1.0\r\n\r\n");
            $response = stream_get_contents($control);
            fclose($control);
            if (!str_starts_with($response, 'HTTP/1.0 200 OK')) {
                die("Control request failed\n");
            }
        }
    }
    fwrite($socket, " HTTP/1.0\r\n\r\n");
    $response = stream_get_contents($socket);
    fclose($socket);
    $body = explode("\r\n\r\n", $response, 2)[1] ?? '';
    echo $name, "\n";
    var_dump(str_starts_with($response, 'HTTP/1.0 200 OK'));
    var_dump($body === "/index.php\ncheck=ok\n" . implode('', $parts));
}
?>
--EXPECT--
unsplit
bool(true)
bool(true)
percent escape
bool(true)
bool(true)
percent digit
bool(true)
bool(true)
parent segment
bool(true)
bool(true)
encoded parent segment
bool(true)
bool(true)
encoded slash
bool(true)
bool(true)
query boundary
bool(true)
bool(true)
large path
bool(true)
bool(true)
