--TEST--
GH-19249 (http context - allow content to be a stream/resource) - no stream
--INI--
allow_url_fopen=1
--CONFLICTS--
server
--FILE--
<?php
$serverCode = '';

include __DIR__."/../../../../sapi/cli/tests/php_cli_server.inc";
php_cli_server_start($serverCode, null, []);

$postData = proc_open("echo", [], $pipes);

$headers = [
    [],
    'header' => [
        'Content-type: application/x-www-form-urlencoded',
    ],
];

foreach ($headers as $header) {
    try {
        file_get_contents("http://" . PHP_CLI_SERVER_ADDRESS . "/", false, stream_context_create([
            'http' => [
                'method' => 'POST',
                ...$header,
                'content' => $postData,
            ]
        ]));
    } catch (TypeError $e) {
        echo $e->getMessage(), "\n";
    }
}

proc_close($postData);
?>
--EXPECT--
file_get_contents(): supplied resource is not a valid stream resource
file_get_contents(): supplied resource is not a valid stream resource
