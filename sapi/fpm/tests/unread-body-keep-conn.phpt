--TEST--
FPM: unread request body cannot become a new request on a kept connection
--SKIPIF--
<?php include "skipif.inc"; ?>
--FILE--
<?php

require_once "tester.inc";

function record(int $type, string $content, int $id): string
{
    return pack('CCnnCC', 1, $type, $id, strlen($content), 0, 0) . $content;
}

function params(array $params): string
{
    $encoded = '';
    $encodeLength = static fn(int $length): string => $length < 128
        ? chr($length)
        : pack('N', $length | 0x80000000);
    foreach ($params as $name => $value) {
        $encoded .= $encodeLength(strlen($name)) . $encodeLength(strlen($value)) . $name . $value;
    }
    return $encoded;
}

function request(int $id, bool $keep, array $params, string $stdin): string
{
    return record(1, pack('nCxxxxx', 1, $keep), $id)
        . record(4, params($params), $id)
        . record(4, '', $id)
        . record(5, $stdin, $id)
        . record(5, '', $id);
}

function transact(string $address, string $request): string
{
    [$host, $port] = explode(':', $address);
    $socket = fsockopen($host, (int) $port);
    stream_set_timeout($socket, 3);
    $written = 0;
    while ($written < strlen($request)) {
        $written += fwrite($socket, substr($request, $written));
    }
    stream_socket_shutdown($socket, STREAM_SHUT_WR);
    $response = stream_get_contents($socket);
    fclose($socket);
    return $response;
}

$config = <<<CFG
[global]
error_log = {{FILE:LOG}}
[unconfined]
listen = {{ADDR}}
pm = static
pm.max_children = 1
catch_workers_output = yes
CFG;

$code = <<<'PHP'
<?php
if ($_SERVER['REMOTE_ADDR'] === '203.0.113.1') {
    echo "smuggled\n";
} else {
    $input = fopen('php://input', 'r');
    echo "outer\n";
}
PHP;

$tester = new FPM\Tester($config, $code);
$script = $tester->makeSourceFile();
$tester->start();
$tester->expectLogStartNotices();

$hiddenParams = [
    'GATEWAY_INTERFACE' => 'FastCGI/1.0',
    'REQUEST_METHOD' => 'GET',
    'SCRIPT_FILENAME' => $script,
    'SCRIPT_NAME' => '/smuggled.php',
    'REQUEST_URI' => '/smuggled.php',
    'SERVER_PROTOCOL' => 'HTTP/1.1',
    'REMOTE_ADDR' => '203.0.113.1',
    'CONTENT_LENGTH' => '0',
];
$hidden = request(2, false, $hiddenParams, '');
$body = str_pad("--B\r\nContent-Disposition: form-data\r\n\r\n", 12000, 'A');
$body = substr_replace($body, $hidden, 5120, strlen($hidden));

$outerParams = [
    'GATEWAY_INTERFACE' => 'FastCGI/1.0',
    'REQUEST_METHOD' => 'POST',
    'SCRIPT_FILENAME' => $script,
    'SCRIPT_NAME' => '/open.php',
    'REQUEST_URI' => '/open.php',
    'SERVER_PROTOCOL' => 'HTTP/1.1',
    'REMOTE_ADDR' => '127.0.0.1',
    'CONTENT_TYPE' => 'multipart/form-data; boundary=B',
    'CONTENT_LENGTH' => (string) strlen($body),
];
foreach ([false, true] as $keep) {
    $response = transact($tester->getAddr(), request(1, $keep, $outerParams, $body));
    echo 'keep ', $keep ? 'on' : 'off', ': ',
        str_contains($response, 'smuggled') ? 'smuggled' : 'safe', "\n";
}

$body = 'A' . $hidden;
$outerParams['CONTENT_TYPE'] = 'application/octet-stream';
$outerParams['CONTENT_LENGTH'] = '1';
$response = transact($tester->getAddr(), request(1, true, $outerParams, $body));
echo 'partial frame: ',
    str_contains($response, 'smuggled') ? 'smuggled' : 'safe', "\n";

$tester->terminate();
$tester->close();
?>
--EXPECT--
keep off: safe
keep on: safe
partial frame: safe
--CLEAN--
<?php
require_once "tester.inc";
FPM\Tester::clean();
?>
