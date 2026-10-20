--TEST--
SOAP client rejects a truncated Content-Length response
--EXTENSIONS--
soap
--SKIPIF--
<?php
if (!function_exists('proc_open')) {
    die('skip proc_open() is not available');
}
?>
--FILE--
<?php
$serverCode = <<<'PHP'
$server = stream_socket_server('tcp://127.0.0.1:0', $errno, $errstr);
if (!$server) {
    fwrite(STDERR, "could not start server: $errstr\n");
    exit(1);
}
echo stream_socket_get_name($server, false), "\n";

$connection = stream_socket_accept($server, 10);
if (!$connection) {
    exit(1);
}

$request = '';
while (!str_contains($request, "\r\n\r\n")) {
    $chunk = fread($connection, 1);
    if ($chunk === '') {
        exit(1);
    }
    $request .= $chunk;
}
preg_match('/Content-Length:\s*(\d+)/i', $request, $matches);
$remaining = (int) $matches[1];
while ($remaining > 0) {
    $chunk = fread($connection, $remaining);
    if ($chunk === '') {
        exit(1);
    }
    $remaining -= strlen($chunk);
}

$body = '<?xml version="1.0"?><SOAP-ENV:Envelope xmlns:SOAP-ENV="http://schemas.xmlsoap.org/soap/envelope/"><SOAP-ENV:Body><testResponse xmlns="urn:test"/></SOAP-ENV:Body></SOAP-ENV:Envelope>';
fwrite($connection, "HTTP/1.1 200 OK\r\n"
    . "Content-Type: text/xml; charset=utf-8\r\n"
    . 'Content-Length: ' . (strlen($body) + 100) . "\r\n"
    . "Connection: keep-alive\r\n"
    . "\r\n"
    . $body);
fclose($connection);
fclose($server);
PHP;

$process = proc_open([PHP_BINARY, '-n', '-r', $serverCode], [
    1 => ['pipe', 'w'],
    2 => ['pipe', 'w'],
], $pipes);
if (!is_resource($process)) {
    die('could not start server process');
}

$address = fgets($pipes[1]);
if ($address === false) {
    die(stream_get_contents($pipes[2]));
}

try {
    $client = new SoapClient(null, [
        'location' => 'http://' . trim($address),
        'uri' => 'urn:test',
        'keep_alive' => true,
    ]);

    try {
        $client->test();
        echo "unexpected success\n";
    } catch (SoapFault $e) {
        echo $e->faultstring, "\n";
    }
} finally {
    fclose($pipes[1]);
    fclose($pipes[2]);
    proc_close($process);
}
?>
--EXPECT--
Error Fetching http body, No Content-Length, connection closed or chunked data
