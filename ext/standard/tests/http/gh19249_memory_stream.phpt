--TEST--
GH-19249 (http context - allow content to be a stream/resource) - memory stream
--INI--
allow_url_fopen=1
--CONFLICTS--
server
--FILE--
<?php
$serverCode = <<<'CODE'
var_dump(getallheaders()['Content-Length']);
echo file_get_contents('php://input');
CODE;

include __DIR__."/../../../../sapi/cli/tests/php_cli_server.inc";
php_cli_server_start($serverCode, null, []);

$postData = fopen("php://memory", "w+");
fwrite($postData, "a=b&c=d");
// Test skip
fseek($postData, 4);

echo file_get_contents("http://" . PHP_CLI_SERVER_ADDRESS . "/", false, stream_context_create([
    'http' => [
        'method' => 'POST',
        'header' => [
        	'Content-type: application/x-www-form-urlencoded',
        ],
        'content' => $postData,
    ]
]));
?>
--EXPECT--
string(1) "3"
c=d
